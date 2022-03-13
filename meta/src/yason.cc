/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/yason>
#include <cc/YasonSyntax>
#include <cc/YasonWriter>
#include <cc/CaptureSink>

namespace cc {

Variant yasonParse(const String &message, const MetaProtocol &protocol)
{
    return YasonSyntax{}.parse(message, protocol);
}

String yasonStringify(const Variant &value)
{
    CaptureSink sink;
    YasonWriter{sink}.write(value);
    return sink.collect();
}

} // namespace cc
