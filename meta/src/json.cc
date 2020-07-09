/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/meta/json>
#include <cc/meta/YasonSyntax>
#include <cc/meta/JsonWriter>
#include <cc/syntax/exceptions>
#include <cc/CaptureSink>

namespace cc {
namespace meta {
namespace json {

Variant parse(const CharArray *text)
{
    return YasonSyntax::instance()->parse(text);
}

String stringify(Variant value)
{
    CaptureSink sink;
    JsonWriter::create(sink)->write(value);
    return sink->collect();
}

}}} // namespace cc::meta::json
