/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/stream/LookAheadStream>

namespace flux {
namespace stream {

Ref<LookAheadStream> LookAheadStream::open(Stream *source, int windowSize)
{
    return new LookAheadStream(source, windowSize);
}

LookAheadStream::LookAheadStream(Stream *source, int windowSize)
    : source_(source),
      window_(ByteArray::create(windowSize)),
      w_(windowSize), m_(0), i_(0),
      done_(false)
{}

bool LookAheadStream::readyRead(double interval) const
{
    if (w_ <= i_) {
        if (done_) return source_->readyRead(interval);
        return true;
    }
    if (i_ < m_) return true;
    return source_->readyRead(interval);
}

int LookAheadStream::read(ByteArray *data)
{
    if (w_ <= i_) {
        if (done_) return source_->read(data);
        return 0;
    }

    if (i_ < m_) {
        int i1 = i_ + data->count();
        if (i1 > m_) i1 = m_;
        *data = *ByteRange(window_, i_, i1);
        int n = i1 - i_;
        i_ = i1;
        return n;
    }

    if (done_) return source_->read(data);

    int r = data->count();
    if (m_ + r > w_) r = w_ - m_;
    int n = source_->read(ByteRange(data, 0, r));
    if (n == 0) return 0;
    *ByteRange(window_, m_, m_ + n) = *data;
    m_ += n;
    i_ += n;
    return n;
}

off_t LookAheadStream::transfer(off_t count, Stream *sink, ByteArray *buf)
{
    if (done_) return source_->transfer(count, sink, buf);
    return Stream::transfer(count, sink, buf);
}

void LookAheadStream::replay()
{
    if (!done_) i_ = 0;
}

void LookAheadStream::done()
{
    i_ = 0;
    done_ = true;
}

}} // namespace flux::stream
