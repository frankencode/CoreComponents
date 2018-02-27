/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/CaptureSink>
#include <cc/meta/YasonSyntax>
#include <cc/meta/JsonWriter>
#include <cc/meta/json>

namespace cc {
namespace meta {
namespace json {

Variant parse(const CharArray *text)
{
    return YasonSyntax::instance()->parse(text);
}

String stringify(Variant value)
{
    Ref<CaptureSink> sink = CaptureSink::open();
    JsonWriter::create(sink)->write(value);
    return sink->collect();
}

}}} // namespace cc::meta::json
