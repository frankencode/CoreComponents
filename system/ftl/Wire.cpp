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

#include "FloatLiteral.hpp"
#include "IntegerLiteral.hpp"
#include "Wire.hpp"

namespace ftl
{

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
			REPEAT(
				CHOICE(
					INLINE("CommentText"),
					GLUE(
						NOT(STRING("*/")),
						ANY()
					)
				)
			),
			STRING("*/")
		)
	);

	DEFINE_VOID("EscapedChar",
		GLUE(
			CHAR('\\'),
			CHOICE(
				RANGE("\"\\/bfnrt"),
				GLUE(
					CHAR('u'),
					REPEAT(4, 4,
						CHOICE(
							RANGE('0', '9'),
							CHOICE(
								RANGE('a', 'f'),
								RANGE('A', 'F')
							)
						)
					)
				)
			)
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
				CHAR('"')
			)
		);

	concatenation_ =
		DEFINE("Concatenation",
			REPEAT(1,
				GLUE(
					REF("String"),
					INLINE("Noise")
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
				CHAR('_'),
				RANGE('0', '9')
			)
		)
	);

	name_ =
		DEFINE("Name",
			CHOICE(
				GLUE(
					NOT(RANGE('0', '9')),
					INLINE("Identifier")
				),
				INLINE("String")
			)
		);

	className_ =
		DEFINE("Class",
			GLUE(
				AHEAD(RANGE('A', 'Z')),
				INLINE("Identifier")
			)
		);

	DEFINE_VOID("Value",
		CHOICE(
			REF("float::Literal"),
			REF("int::Literal"),
			REF("Concatenation"),
			REF("Object"),
			REF("Array"),
			REF("SpecialValue")
		)
	);

	array_ =
		DEFINE("Array",
			GLUE(
				CHAR('['),
				INLINE("Noise"),
				REPEAT(0, 1,
					GLUE(
						INLINE("Value"),
						INLINE("Noise"),
						REPEAT(
							GLUE(
								REPEAT(0, 1,
									GLUE(
										CHAR(','),
										INLINE("Noise")
									)
								),
								INLINE("Value"),
								INLINE("Noise")
							)
						)
					)
				),
				CHAR(']')
			)
		);

	DEFINE_VOID("Member",
		GLUE(
			REF("Name"),
			INLINE("Noise"),
			CHAR(':'),
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
				INLINE("Noise"),
				REPEAT(0, 1,
					GLUE(
						INLINE("Member"),
						INLINE("Noise"),
						REPEAT(
							GLUE(
								REPEAT(0, 1,
									GLUE(
										CHAR(';'),
										INLINE("Noise")
									)
								),
								INLINE("Member"),
								INLINE("Noise")
							)
						)
					)
				),
				CHAR('}')
			)
		);

	message_ =
		DEFINE("Message",
			GLUE(
				INLINE("Noise"),
				REF("Object"),
				INLINE("Noise")
			)
		);

	ENTRY("Message");
	LINK();
}

Ref<WireObject, Owner> Wire::parse(Ref<ByteArray> text)
{
	int i0 = 0, i1 = 0;
	Ref<Token, Owner> token = match(text, i0, &i1);
	FTL_CHECK(token, WireException, "Invalid syntax");
	return parseObject(text, token->firstChild());
}

String Wire::parseConcatenation(Ref<ByteArray> text, Ref<Token> token)
{
	StringList l;
	token = token->firstChild();
	while (token) {
		l.append(text->copy(token->i0() + 1, token->i1() - 1));
		token = token->nextSibling();
	}
	return (l.length() == 1) ? l.at(0) : l.join();
}

Ref<WireObject, Owner> Wire::parseObject(Ref<ByteArray> text, Ref<Token> token)
{
	Ref<WireObject, Owner> object = new WireObject;
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
		token = token->nextSibling();
		Variant value = parseValue(text, token);
		object->insert(name, value);
		token = token->nextSibling();
	}
	return object;
}

Ref<WireArray, Owner> Wire::parseArray(Ref<ByteArray> text, Ref<Token> token)
{
	Ref<WireArray, Owner> array = new WireArray(token->countChildren());
	int i = 0;
	for (Ref<Token> child = token->firstChild(); child; child = child->nextSibling()) {
		array->set(i, parseValue(text, child));
		++i;
	}
	return array;
}

Variant Wire::parseValue(Ref<ByteArray> text, Ref<Token> token)
{
	Variant value;

	if (token->definition() == floatLiteral()->id()) {
		value = floatLiteral()->read(text, token);
	}
	else if (token->definition() == integerLiteral()->id()) {
		uint64_t x; int s;
		integerLiteral()->read(text, token, &x, &s);
		value = int(x) * s;
	}
	else if (token->rule() == concatenation_) {
		value = parseConcatenation(text, token);
	}
	else if (token->rule() == object_) {
		value = parseObject(text, token);
	}
	else if (token->rule() == array_) {
		value = parseArray(text, token);
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
