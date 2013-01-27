 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
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

	DEFINE_VOID("begin-list",
		GLUE(
			INLINE("ws"),
			CHAR('['),
			INLINE("ws")
		)
	);

	DEFINE_VOID("end-list",
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
				REF("list"),
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

	list_ =
		DEFINE("list",
			GLUE(
				INLINE("begin-list"),
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
				INLINE("end-list")
			)
		);

	text_ =
		DEFINE("JSON-text",
			CHOICE(
				REF("object"),
				REF("list")
			)
		);

	ENTRY("JSON-text");
	LINK();
}

Variant Json::parse(String text)
{
	O<Token> token = match(text);
	FTL_CHECK(token, JsonException, "Invalid syntax");
	return parseText(text, token);
}

Variant Json::parseText(ByteArray *text, Token *token)
{
	FTL_CHECK(token->rule() == text_, JsonException, "");
	Token *child = token->firstChild();
	return (child->rule() == object_) ? parseObject(text, child) : parseArray(text, child);
}

Variant Json::parseObject(ByteArray *text, Token *token)
{
	FTL_CHECK(token->rule() == object_, JsonException, "");
	O<JsonObject> object = JsonObject::create();
	for (Token *child = token->firstChild(); child; child = child->nextSibling()) {
		Pair<String, Variant> member = parseMember(text, child);
		object->insert(member.key(), member.value());
	}
	return object;
}

Variant Json::parseArray(ByteArray *text, Token *token)
{
	FTL_CHECK(token->rule() == list_, JsonException, "");
	O<VariantList> list = VariantList::create(token->countChildren());
	int i = 0;
	for (Token *child = token->firstChild(); child; child = child->nextSibling()) {
		list->set(i, parseValue(text, child));
		++i;
	}
	return list;
}

Pair<String, Variant> Json::parseMember(ByteArray *text, Token *token)
{
	FTL_CHECK(token->rule() == member_, JsonException, "");
	FTL_CHECK(token->countChildren() == 2, JsonException, "");
	return Pair<String, Variant>(
		parseString(text, token->firstChild()),
		parseValue(text, token->lastChild())
	);
}

Variant Json::parseValue(ByteArray *text, Token *token)
{
	FTL_CHECK(token->rule() == value_, JsonException, "");
	Variant value;
	if (token->hasChildren()) {
		FTL_CHECK(token->countChildren() == 1, JsonException, "");
		Token *child = token->firstChild();
		if (child->rule() == object_)
			value = parseObject(text, child);
		else if (child->rule() == list_)
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

String Json::parseString(ByteArray *text, Token *token)
{
	FTL_CHECK(token->rule() == string_, JsonException, "");
	return text->copy(token->i0() + 1, token->i1() - 1);
}

double Json::parseNumber(ByteArray *text, Token *token)
{
	FTL_CHECK(token->rule() == number_, JsonException, "");
	bool ok = true;
	double value = String(text->copy(token->i0(), token->i1()))->toFloat(&ok);
	FTL_CHECK(ok, JsonException, "Malformed number");
	return value;
}

} // namespace ftl
