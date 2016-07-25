/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Format>
#include <cc/meta/YasonSyntax>
#include <cc/meta/YasonWriter>
#include <cc/meta/yason>

namespace cc {
namespace yason {

Variant parse(const ByteArray *text, const MetaProtocol *protocol)
{
    return YasonSyntax::instance()->parse(text, protocol);
}

String stringify(Variant value)
{
    Format format;
    YasonWriter::create(format)->write(value);
    return format;
}

}} // namespace cc::yason
