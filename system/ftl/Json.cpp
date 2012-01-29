/*
 * Json.cpp --  a JSON parser according to RFC 4627
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */

#include "Json.hpp"

namespace ftl
{

Json::Json()
{
	DEFINE_VOID("ws", REPEAT(RANGE(" \t\n\r")));
	
	DEFINE_VOID("begin-object",
		GLUE(
			INLINE("ws"),
			CHAR('{'),
			INLINE("ws")
		)
	);
	
	DEFINE_VOID("end-object",
		GLUE(
			INLINE("ws"),
			CHAR('}'),
			INLINE("ws")
		)
	);
	
	DEFINE_VOID("begin-array",
		GLUE(
			INLINE("ws"),
			CHAR('['),
			INLINE("ws")
		)
	);
	
	DEFINE_VOID("end-array",
		GLUE(
			INLINE("ws"),
			CHAR(']'),
			INLINE("ws")
		)
	);
	
	DEFINE_VOID("name-separator",
		GLUE(
			INLINE("ws"),
			CHAR(':'),
			INLINE("ws")
		)
	);
	
	DEFINE_VOID("value-separator",
		GLUE(
			INLINE("ws"),
			CHAR(','),
			INLINE("ws")
		)
	);
	
	int_ =
		DEFINE("int",
			CHOICE(
				CHAR('0'),
				GLUE(
					RANGE('1', '9'),
					REPEAT(RANGE('0', '9'))
				)
			)
		);
	
	frac_ =
		DEFINE("frac",
			GLUE(
				CHAR('.'),
				REPEAT(1, RANGE('0', '9'))
			)
		);
	
	exp_ =
		DEFINE("exp",
			GLUE(
				RANGE("eE"),
				REPEAT(0, 1, RANGE("+-")),
				REPEAT(1, RANGE('0', '9'))
			)
		);
	
	number_ =
		DEFINE("number",
			CHOICE(
				GLUE(
					REPEAT(0, 1, CHAR('-')),
					REF("int"),
					REPEAT(0, 1, REF("frac")),
					REPEAT(0, 1, REF("exp"))
				),
				KEYWORD("nan inf NaN INFINITE")
					// minor deviation from the standard: also allow Nan and inf
			)
		);
	
	string_ =
		DEFINE("string",
			GLUE(
				CHAR('"'),
				REPEAT(
					CHOICE(
						GLUE(
							CHAR('\\'),
							CHOICE(
								RANGE("\"\\/bfnrt"),
								GLUE(
									CHAR('u'),
									REPEAT(4, 4, INLINE("HEXDIG"))
								)
							)
						),
						OTHER('"')
							// minor deviation from standard for two reasons:
							// make it work on raw bytes and tolerate non-standard escapes
					)
				),
				CHAR('"')
			)
		);
	
	value_ =
		DEFINE("value",
			CHOICE(
				KEYWORD("true false null"),
				REF("object"),
				REF("array"),
				REF("number"),
				REF("string")
			)
		);
	true_ = keywordByName("true");
	false_ = keywordByName("false");
	null_ = keywordByName("null");
	
	member_ =
		DEFINE("member",
			GLUE(
				REF("string"),
				INLINE("name-separator"),
				REF("value")
			)
		);
	
	object_ =
		DEFINE("object",
			GLUE(
				INLINE("begin-object"),
				REPEAT(0, 1,
					GLUE(
						REF("member"),
						REPEAT(
							GLUE(
								INLINE("value-separator"),
								REF("member")
							)
						)
					)
				),
				INLINE("end-object")
			)
		);
	
	array_ =
		DEFINE("array",
			GLUE(
				INLINE("begin-array"),
				REPEAT(0, 1,
					GLUE(
						REF("value"),
						REPEAT(
							GLUE(
								INLINE("value-separator"),
								REF("value")
							)
						)
					)
				),
				INLINE("end-array")
			)
		);
	
	text_ =	
		DEFINE("JSON-text",
			CHOICE(
				REF("object"),
				REF("array")
			)
		);
	
	ENTRY("JSON-text");
#ifndef NDEBUG
	LINK();
#endif
}

Variant Json::parse(String text)
{
	int i0 = 0, i1 = 0;
	Ref<Token, Owner> token = match(text, i0, &i1);
	FTL_CHECK(token, JsonException, "Invalid syntax");
	return parseText(text, token);
}

Variant Json::parseText(Ref<ByteArray> text, Ref<Token> token)
{
	FTL_CHECK(token->rule() == text_, JsonException, "");
	Ref<Token> child = token->firstChild();
	return (child->rule() == object_) ? parseObject(text, child) : parseArray(text, child);
}

Variant Json::parseObject(Ref<ByteArray> text, Ref<Token> token)
{
	FTL_CHECK(token->rule() == object_, JsonException, "");
	Ref<JsonObject, Owner> object = new JsonObject;
	for (Ref<Token> child = token->firstChild(); child; child = child->nextSibling()) {
		Pair<String, Variant> member = parseMember(text, child);
		object->insert(member.key(), member.value());
	}
	return object;
}

Variant Json::parseArray(Ref<ByteArray> text, Ref<Token> token)
{
	FTL_CHECK(token->rule() == array_, JsonException, "");
	Ref<JsonArray, Owner> array = new JsonArray(token->countChildren());
	int i = 0;
	for (Ref<Token> child = token->firstChild(); child; child = child->nextSibling()) {
		array->set(i, parseValue(text, child));
		++i;
	}
	return array;
}

Pair<String, Variant> Json::parseMember(Ref<ByteArray> text, Ref<Token> token)
{
	FTL_CHECK(token->rule() == member_, JsonException, "");
	FTL_CHECK(token->countChildren() == 2, JsonException, "");
	return Pair<String, Variant>(
		parseString(text, token->firstChild()),
		parseValue(text, token->lastChild())
	);
}

Variant Json::parseValue(Ref<ByteArray> text, Ref<Token> token)
{
	FTL_CHECK(token->rule() == value_, JsonException, "");
	Variant value;
	if (token->hasChildren()) {
		FTL_CHECK(token->countChildren() == 1, JsonException, "");
		Ref<Token> child = token->firstChild();
		if (child->rule() == object_)
			value = parseObject(text, child);
		else if (child->rule() == array_)
			value = parseArray(text, child);
		else if (child->rule() == string_)
			value = parseString(text, child);
		else if (child->rule() == number_)
			value = parseNumber(text, child);
		else
			FTL_CHECK(false, JsonException, "Impossible branch");
	}
	else {
		if (token->keyword() == true_)
			value = true;
		else if (token->keyword() == false_)
			value = false;
		else if (token->keyword() == null_)
			;
		else
			FTL_CHECK(false, JsonException, "Impossible branch");
	}
	return value;
}

String Json::parseString(Ref<ByteArray> text, Ref<Token> token)
{
	FTL_CHECK(token->rule() == string_, JsonException, "");
	return text->copy(token->i0() + 1, token->i1() - 1);
}

double Json::parseNumber(Ref<ByteArray> text, Ref<Token> token)
{
	FTL_CHECK(token->rule() == number_, JsonException, "");
	bool ok = true;
	double value = String(text->copy(token->i0(), token->i1()))->toFloat(&ok);
	FTL_CHECK(ok, JsonException, "Malformed number");
	return value;
}

} // namespace ftl
