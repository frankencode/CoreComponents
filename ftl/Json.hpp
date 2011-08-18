/*
 * Json.hpp -- a JSON parser according to RFC 4627
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_JSON_HPP
#define FTL_JSON_HPP

#include "AbnfCoreSyntax.hpp"
#include "ThreadLocalSingleton.hpp"
#include "Variant.hpp"
#include "Array.hpp"
#include "Map.hpp"

namespace ftl
{

FTL_EXCEPTION(JsonException, Exception);

typedef Array<Variant> JsonArray;
typedef Map<String, Variant> JsonObject;

class Json: public AbnfCoreSyntax, public ThreadLocalSingleton<Json>
{
public:
	Variant parse(String text);
	
private:
	friend class ThreadLocalSingleton<Json>;
	Json();
	Variant parseText(Ref<ByteArray> text, Ref<Token> token);
	Variant parseObject(Ref<ByteArray> text, Ref<Token> token);
	Variant parseArray(Ref<ByteArray> text, Ref<Token> token);
	Pair<String, Variant> parseMember(Ref<ByteArray> text, Ref<Token> token);
	Variant parseValue(Ref<ByteArray> text, Ref<Token> token);
	String parseString(Ref<ByteArray> text, Ref<Token> token);
	double parseNumber(Ref<ByteArray> text, Ref<Token> token);
	
	int int_;
	int frac_;
	int exp_;
	int number_;
	int string_;
	int value_;
	int member_;
	int object_;
	int array_;
	int text_;
	
	int false_, true_, null_;
};

inline Ref<Json> json() { return Json::instance(); }

} // namespace ftl

#endif // FTL_JSON_HPP
