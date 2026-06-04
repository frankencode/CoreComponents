/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the Apache License version 2.0
 * (see CoreComponents/LICENSE-Apache-2.0).
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
