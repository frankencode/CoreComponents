/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/TransferLimiter>

namespace cc {

Ref<TransferLimiter> TransferLimiter::open(Stream *stream, size_t readLimit, size_t writeLimit)
{
    return new TransferLimiter(stream, readLimit, writeLimit);
}

TransferLimiter::TransferLimiter(Stream *stream, size_t readLimit, size_t writeLimit):
    stream_(stream),
    readLimit_(readLimit),
    writeLimit_(writeLimit),
    totalRead_(0),
    totalWritten_(0)
{}

int TransferLimiter::read(CharArray *data)
{
    if (readLimit_ > 0 && totalRead_ >= readLimit_)
        throw ReadLimitExceeded();
    int n = stream_->read(data);
    totalRead_ += n;
    return n;
}

void TransferLimiter::write(const CharArray *data)
{
    if (writeLimit_ > 0 && totalWritten_ + data->count() > writeLimit_)
        throw WriteLimitExceeded();
    write(data);
    totalWritten_ += data->count();
}

void TransferLimiter::write(const StringList *parts)
{
    size_t h = 0;
    for (int i = 0, n = parts->count(); i < n; ++i)
        h += parts->at(i)->count();
    if (totalWritten_ + h > writeLimit_) throw WriteLimitExceeded();
    write(parts);
    totalWritten_ += h;
}

} // namespace cc
