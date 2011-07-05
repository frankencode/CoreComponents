/*
 * Json.cpp -- JSON parsing and stringification
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */

#include "JsonSyntax.hpp"
#include "Json.hpp"

namespace ftl
{

Json::Json()
{}

Variant Json::parseText(Ref<ByteArray> text, Ref<Token> token)
{
	check<JsonException>(token->rule() == syntax_.text_);
	Ref<Token> child = token->firstChild();
	return (child->rule() == syntax_.object_) ? parseObject(text, child) : parseArray(text, child);
}

Variant Json::parseObject(Ref<ByteArray> text, Ref<Token> token)
{
	check<JsonException>(token->rule() == syntax_.object_);
	Ref<JsonObject, Owner> object = new JsonObject;
	for (Ref<Token> child = token->firstChild(); child; child = child->nextSibling()) {
		Pair<String, Variant> member = parseMember(text, child);
		object->insert(member.key(), member.value());
	}
	return object;
}

Variant Json::parseArray(Ref<ByteArray> text, Ref<Token> token)
{
	check<JsonException>(token->rule() == syntax_.array_);
	Ref<JsonArray, Owner> array = new JsonArray(token->countSiblings());
	int i = 0;
	for (Ref<Token> child = token->firstChild(); child; child = child->nextSibling()) {
		array->set(i, parseValue(text, child));
		++i;
	}
	return array;
}

Pair<String, Variant> Json::parseMember(Ref<ByteArray> text, Ref<Token> token)
{
	check<JsonException>(token->rule() == syntax_.member_);
	check<JsonException>(token->countChildren() == 2);
	return Pair<String, Variant>(
		parseString(text, token->firstChild()),
		parseValue(text, token->lastChild())
	);
}

Variant Json::parseValue(Ref<ByteArray> text, Ref<Token> token)
{
	check<JsonException>(token->rule() == syntax_.value_);
	return Variant();
}

String Json::parseString(Ref<ByteArray> text, Ref<Token> token)
{
	check<JsonException>(token->rule() == syntax_.string_);
	String s = text->copy(token->i0() + 1, token->i1() - 1);
	s.expandInsitu();
	return s;
}

Variant Json::parse(String text)
{
	int i0 = 0, i1 = 0;
	Ref<Token, Owner> token = syntax_.match(text, i0, &i1);
	check<JsonException>(token, "Invalid syntax");
	return parseText(text, token);
}

String Json::stringify(Variant value)
{
	return String();
}

} // namespace ftl
