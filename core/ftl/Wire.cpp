/*
 * Wire.cpp -- the Wire parser
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */

#include "Format.hpp"
#include "FloatLiteral.hpp"
#include "IntegerLiteral.hpp"
#include "Wire.hpp"

namespace ftl
{

WireException::WireException(const String &error, int line, int pos)
	: error_(error),
	  line_(line),
	  pos_(pos)
{
	message_ = Format("%%:%%: %%") << line << pos << error;
}

WireException::~WireException() throw() {}

const char *WireException::what() const throw() { return message_; }

Wire::Wire()
{
	SYNTAX("Wire");

	IMPORT(floatLiteral(), "float");
	IMPORT(integerLiteral(), "int");

	DEFINE_VOID("CommentLine",
		GLUE(
			STRING("//"),
			CHOICE(
				FIND(AHEAD(CHAR('\n'))),
				FIND(EOI())
			)
		)
	);

	DEFINE_VOID("CommentText",
		GLUE(
			STRING("/*"),
			HINT("Unterminated comment text"),
			REPEAT(
				CHOICE(
					INLINE("CommentText"),
					GLUE(
						NOT(STRING("*/")),
						ANY()
					)
				)
			),
			STRING("*/"),
			DONE()
		)
	);

	DEFINE_VOID("EscapedChar",
		GLUE(
			CHAR('\\'),
			HINT("Illegal escape sequence"),
			CHOICE(
				RANGE("\"\\/fnrt"),
				GLUE(
					CHAR('u'),
					REPEAT(4, 4,
						CHOICE(
							RANGE('0', '9'),
							RANGE('a', 'f'),
							RANGE('A', 'F')
						)
					)
				)
			),
			DONE()
		)
	);

	DEFINE_VOID("Noise",
		REPEAT(
			CHOICE(
				RANGE(" \t\n\r"),
				INLINE("CommentLine"),
				INLINE("CommentText")
			)
		)
	);

	string_ =
		DEFINE("String",
			GLUE(
				CHAR('"'),
				REPEAT(
					CHOICE(
						INLINE("EscapedChar"),
						EXCEPT("\"\n")
					)
				),
				HINT("Unterminated string, expected '\"'"),
				CHAR('"'),
				DONE()
			)
		);

	concatenation_ =
		DEFINE("Concatenation",
			GLUE(
				REF("String"),
				REPEAT(
					GLUE(
						INLINE("Noise"),
						REF("String")
					)
				)
			)
		);

	specialValue_ =
		DEFINE("SpecialValue",
			KEYWORD("true false null")
		);

	true_ = keywordByName("true");
	false_ = keywordByName("false");
	null_ = keywordByName("null");

	DEFINE_VOID("Identifier",
		REPEAT(1,
			CHOICE(
				RANGE('a', 'z'),
				RANGE('A', 'Z'),
				RANGE("_-"),
				RANGE('0', '9')
			)
		)
	);

	name_ =
		DEFINE("Name",
			GLUE(
				HINT("Invalid name"),
				CHOICE(
					GLUE(
						NOT(RANGE('0', '9')),
						INLINE("Identifier")
					),
					INLINE("String")
				),
				DONE()
			)
		);

	className_ =
		DEFINE("Class",
			GLUE(
				HINT("Invalid class name"),
				AHEAD(RANGE('A', 'Z')),
				INLINE("Identifier"),
				DONE()
			)
		);

	DEFINE_VOID("Value",
		CHOICE(
			REF("int::Literal"),
			REF("float::Literal"),
			REF("Concatenation"),
			REF("Object"),
			REF("List"),
			REF("SpecialValue"),
			GLUE(
				HINT("Invalid value"),
				FAIL()
			)
		)
	);

	list_ =
		DEFINE("List",
			GLUE(
				CHAR('['),
				INLINE("Noise"),
				REPEAT(0, 1,
					GLUE(
						INLINE("Value"),
						REPEAT(
							GLUE(
								HINT("Invalid value separator, expected ',' or whitespace"),
								LENGTH(1,
									GLUE(
										INLINE("Noise"),
										REPEAT(0, 1,
											GLUE(
												CHAR(','),
												INLINE("Noise")
											)
										)
									)
								),
								DONE(),
								INLINE("Value")
							)
						),
						INLINE("Noise")
					)
				),
				HINT("Unterminated list, expected ']'"),
				CHAR(']'),
				DONE()
			)
		);

	DEFINE_VOID("Member",
		GLUE(
			REF("Name"),
			INLINE("Noise"),
			HINT("Expected ':'"),
			CHAR(':'),
			DONE(),
			INLINE("Noise"),
			INLINE("Value")
		)
	);

	object_ =
		DEFINE("Object",
			GLUE(
				REPEAT(0, 1,
					GLUE(
						REF("Class"),
						INLINE("Noise")
					)
				),
				CHAR('{'),
				HINT("Unterminated object"),
				INLINE("Noise"),
				REPEAT(0, 1,
					GLUE(
						INLINE("Member"),
						REPEAT(
							GLUE(
								HINT("Invalid member separator, expected ';' or whitespace"),
								LENGTH(1,
									GLUE(
										INLINE("Noise"),
										REPEAT(0, 1,
											GLUE(
												CHAR(';'),
												INLINE("Noise")
											)
										)
									)
								),
								DONE(),
								INLINE("Member")
							)
						),
						INLINE("Noise")
					)
				),
				CHAR('}'),
				DONE()
			)
		);

	message_ =
		DEFINE("Message",
			GLUE(
				INLINE("Noise"),
				INLINE("Value"),
				INLINE("Noise")
			)
		);

	ENTRY("Message");
	LINK();
}

Variant Wire::parse(Ref<ByteArray> text, Ref<WireObject> virgin)
{
	Ref<SyntaxState, Owner> state = newState();
	Ref<Token, Owner> token = match(text, -1, state);
	if (!token) {
		String reason = "Syntax error";
		int line = 1, pos = 1;
		if (state->hint()) {
			reason = state->hint();
			text->offsetToLinePos(state->hintOffset(), &line, &pos);
		}
		throw WireException(reason, line, pos);
	}
	Ref<Token> child = token->firstChild();
	if (virgin) {
		if (child->rule() != object_)
			throw WireException("Expected an object value", 0, 0);
		parseObject(text, child, virgin);
		return virgin;
	}
	return parseValue(text, child);
}

String Wire::parseConcatenation(Ref<ByteArray> text, Ref<Token> token)
{
	Ref<StringList, Owner> l = StringList::create();
	token = token->firstChild();
	while (token) {
		l << text->copy(token->i0() + 1, token->i1() - 1);
		token = token->nextSibling();
	}
	return (l->length() == 1) ? l->at(0) : l->join();
}

Ref<WireObject, Owner> Wire::parseObject(Ref<ByteArray> text, Ref<Token> token, Ref<WireObject> virgin)
{
	Ref<WireObject, Owner> object = virgin;
	if (!object) object = new WireObject;
	token = token->firstChild();
	if (token) {
		if (token->rule() == className_) {
			object->className_ = text->copy(token->i0(), token->i1());
			token = token->nextSibling();
		}
	}
	while (token) {
		bool stripQuotation = (text->at(token->i0()) == '"');
		String name = text->copy(token->i0() + stripQuotation, token->i1() - stripQuotation);
		if (object->contains(name)) {
			int line = 1, pos = 1;
			text->offsetToLinePos(token->i1(), &line, &pos);
			throw WireException(Format("Ambiguous member name \"%%\"") << name, line, pos);
		}
		token = token->nextSibling();
		Variant value = parseValue(text, token);
		object->insert(name, value);
		token = token->nextSibling();
	}
	return object;
}

Ref<VariantList, Owner> Wire::parseList(Ref<ByteArray> text, Ref<Token> token)
{

	Ref<VariantList, Owner> list = VariantList::create(token->countChildren());
	int i = 0;
	for (Ref<Token> child = token->firstChild(); child; child = child->nextSibling()) {
		list->set(i, parseValue(text, child));
		++i;
	}
	return list;
}

Variant Wire::parseValue(Ref<ByteArray> text, Ref<Token> token)
{
	Variant value;

	if (token->definition() == floatLiteral()->id()) {
		float64_t x;
		floatLiteral()->read(&x, text, token);
		value = x;
	}
	else if (token->definition() == integerLiteral()->id()) {
		uint64_t x; int s;
		integerLiteral()->read(&x, &s, text, token);
		value = int(x) * s;
	}
	else if (token->rule() == concatenation_) {
		value = parseConcatenation(text, token);
	}
	else if (token->rule() == object_) {
		value = parseObject(text, token);
	}
	else if (token->rule() == list_) {
		value = parseList(text, token);
	}
	else if (token->rule() == specialValue_) {
		if (token->keyword() == true_)
			value = true;
		else if (token->keyword() == false_)
			value = false;
	}
	return value;
}

} // namespace ftl
