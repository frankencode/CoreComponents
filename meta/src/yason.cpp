/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/Format>
#include <flux/meta/YasonSyntax>
#include <flux/meta/YasonWriter>
#include <flux/meta/yason>

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
