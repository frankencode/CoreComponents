/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/Singleton>
#include <flux/NullStream>

namespace flux {

NullStream::NullStream()
{}

int NullStream::read(ByteArray *buf)
{
    if (buf->count() == 0) buf->resize(0xFFF);
    else buf->clear();
    return buf->count();
}

void NullStream::write(const ByteArray *buf)
{}

Stream *nullStream() { return Singleton<NullStream>::instance(); }

} // namespace flux
