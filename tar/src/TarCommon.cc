/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/tar/TarCommon>

namespace cc {
namespace tar {

unsigned tarHeaderSum(const ByteArray *data)
{
    unsigned sum = 0;
    for (int i = 0;       i < 148 && i < data->count(); ++i) sum += data->byteAt(i);
    for (int i = 0 + 148; i < 156 && i < data->count(); ++i) sum += ' ';
    for (int i = 0 + 156; i < 512 && i < data->count(); ++i) sum += data->byteAt(i);
    return sum;
}

}} // namespace cc::tar
