/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/CaptureSink>
#include <cc/meta/YasonSyntax>
#include <cc/meta/YasonWriter>
#include <cc/meta/yason>

namespace cc {
namespace meta {
namespace yason {

Variant parse(const CharArray *text, const MetaProtocol *protocol)
{
    return YasonSyntax::instance()->parse(text, protocol);
}

String stringify(Variant value)
{
    Ref<CaptureSink> sink = CaptureSink::open();
    YasonWriter::create(sink)->write(value);
    return sink->collect();
}

}}} // namespace cc::meta::yason
