/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/Format>
#include "ChunkedSink.h"

namespace fluxnode {

Ref<ChunkedSink> ChunkedSink::open(Stream *stream)
{
    return new ChunkedSink(stream);
}

ChunkedSink::ChunkedSink(Stream *stream)
    : stream_(stream)
{}

ChunkedSink::~ChunkedSink()
{
    Format(stream_) << 0 << "\r\n" << "\r\n";
}

void ChunkedSink::write(const ByteArray *buf)
{
    Format(stream_) << hex(buf->count()) << "\r\n" << buf << "\r\n";
}

void ChunkedSink::write(const StringList *parts)
{
    Format chunk(stream_);
    int total = 0;
    for (int i = 0; i < parts->count(); ++i)
        total += parts->at(i)->count();
    chunk << hex(total) << "\r\n";
    for (int i = 0; i < parts->count(); ++i)
        chunk << parts->at(i);
    chunk << "\r\n";
}

} // namespace fluxnode
