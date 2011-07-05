/*
 * Json.hpp -- JSON parsing and stringification
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_JSON_HPP
#define FTL_JSON_HPP

#include "atoms"
#include "ThreadLocalSingleton.hpp"
#include "Variant.hpp"
#include "Array.hpp"
#include "Map.hpp"

namespace ftl
{

class JsonSyntax;

FTL_EXCEPTION(JsonException, Exception);

typedef Array<Variant> JsonArray;
typedef Map<String, Variant> JsonObject;

class Json: public Instance, public ThreadLocalSingleton<Json>
{
public:
	Variant parse(String text);
	String stringify(Variant value);
	
private:
	friend class ThreadLocalSingleton<Json>;
	Json();
	Variant parseText(Ref<ByteArray> text, Ref<Token> token);
	Variant parseObject(Ref<ByteArray> text, Ref<Token> token);
	Variant parseArray(Ref<ByteArray> text, Ref<Token> token);
	Pair<String, Variant> parseMember(Ref<ByteArray> text, Ref<Token> token);
	Variant parseValue(Ref<ByteArray> text, Ref<Token> token);
	String parseString(Ref<ByteArray> text, Ref<Token> token);
	JsonSyntax syntax_;
};

inline Ref<Json> json() { return Json::instance(); }

} // namespace ftl

#endif // FTL_JSON_HPP
