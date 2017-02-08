/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Stream>

namespace cc {

/** \brief Transfer measuring stream
  */
class TransferMeter: public Stream
{
public:
    static Ref<TransferMeter> open(Stream *stream);

    inline Stream *stream() const { return stream_; }
    inline size_t totalRead() const { return totalRead_; }
    inline size_t totalWritten() const { return totalWritten_; }

    virtual int read(ByteArray *buf);
    virtual void write(const ByteArray *buf);
    virtual void write(const StringList *parts);

private:
    TransferMeter(Stream *stream);

    Ref<Stream> stream_;
    size_t totalRead_;
    size_t totalWritten_;
};

} // namespace cc
