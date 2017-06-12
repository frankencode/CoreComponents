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
  * \see Transfer
  */
class TransferMeter: public Stream
{
public:
    static Ref<TransferMeter> open(Stream *stream);

    inline Stream *stream() const { return stream_; }
    inline off_t totalRead() const { return totalRead_; }
    inline off_t totalWritten() const { return totalWritten_; }

    virtual int read(ByteArray *buf) override;
    virtual void write(const ByteArray *buf) override;
    virtual void write(const StringList *parts) override;

protected:
    TransferMeter(Stream *stream);

private:
    Ref<Stream> stream_;
    off_t totalRead_;
    off_t totalWritten_;
};

} // namespace cc
