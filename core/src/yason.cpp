/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/Format>
#include <flux/YasonSyntax>
#include <flux/YasonWriter>
#include <flux/yason>

namespace flux {
namespace yason {

Variant parse(const ByteArray *text, const MetaProtocol *protocol)
{
    return yasonSyntax()->parse(text, protocol);
}

String stringify(Variant value)
{
    Format format;
    YasonWriter::create(format)->write(value);
    return format;
}

}} // namespace flux::yason
