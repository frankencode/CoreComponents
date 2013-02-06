 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */

#include "format.hpp"
#include "FloatLiteral.hpp"
#include "IntegerLiteral.hpp"
#include "Wire.hpp"

namespace ftl
{

WireException::WireException(const string &error, int line, int pos)
	: error_(error),
	  line_(line),
	  pos_(pos)
{
	message_ = format("%%:%%: %%") << line << pos << error;
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
		DEFINE("string",
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
				REF("string"),
				REPEAT(
					GLUE(
						INLINE("Noise"),
						REF("string")
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
					INLINE("string")
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

variant Wire::parse(ByteArray *text, WireObject *virgin)
{
	Ref<SyntaxState> state = newState();
	Ref<Token> token = match(text, -1, state);
	if (!token) {
		string reason = "Syntax error";
		int line = 1, pos = 1;
		if (state->hint()) {
			reason = state->hint();
			text->offsetToLinePos(state->hintOffset(), &line, &pos);
		}
		throw WireException(reason, line, pos);
	}
	Token *child = token->firstChild();
	if (virgin) {
		if (child->rule() != object_)
			throw WireException("Expected an object value", 0, 0);
		parseObject(text, child, virgin);
		return virgin;
	}
	return parseValue(text, child);
}

string Wire::parseConcatenation(ByteArray *text, Token *token)
{
	Ref<StringList> l = StringList::create();
	token = token->firstChild();
	while (token) {
		*l << text->copy(token->i0() + 1, token->i1() - 1);
		token = token->nextSibling();
	}
	return (l->length() == 1) ? l->at(0) : l->join();
}

Ref<WireObject> Wire::parseObject(ByteArray *text, Token *token, WireObject *virgin)
{
	Ref<WireObject> object = virgin;
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
		string name = text->copy(token->i0() + stripQuotation, token->i1() - stripQuotation);
		if (object->contains(name)) {
			int line = 1, pos = 1;
			text->offsetToLinePos(token->i1(), &line, &pos);
			throw WireException(format("Ambiguous member name \"%%\"") << name, line, pos);
		}
		token = token->nextSibling();
		variant value = parseValue(text, token);
		object->insert(name, value);
		token = token->nextSibling();
	}
	return object;
}

Ref<VariantList> Wire::parseList(ByteArray *text, Token *token)
{

	Ref<VariantList> list = VariantList::create(token->countChildren());
	int i = 0;
	for (Token *child = token->firstChild(); child; child = child->nextSibling()) {
		list->set(i, parseValue(text, child));
		++i;
	}
	return list;
}

variant Wire::parseValue(ByteArray *text, Token *token)
{
	variant value;

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
