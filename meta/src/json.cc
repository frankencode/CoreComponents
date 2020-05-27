/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/CaptureSink>
#include <cc/syntax/exceptions>
#include <cc/meta/YasonSyntax>
#include <cc/meta/JsonWriter>
#include <cc/meta/json>

namespace cc {
namespace meta {
namespace json {

variant parse(const CharArray *text)
{
    return YasonSyntax::instance()->parse(text);
}

string stringify(variant value)
{
    Ref<CaptureSink> sink = CaptureSink::open();
    JsonWriter::create(sink)->write(value);
    return sink->collect();
}

}}} // namespace cc::meta::json
