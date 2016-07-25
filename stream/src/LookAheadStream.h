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
namespace stream {

/** \brief Look-a-head stream buffer
  */
class LookAheadStream: public Stream
{
public:
    static Ref<LookAheadStream> open(Stream *source, int windowSize);

    int read(ByteArray *data) override;
    off_t transferSpanTo(off_t count = -1, Stream *sink = 0, ByteArray *buf = 0) override;

    void replay();
    void done();

private:
    LookAheadStream(Stream *source, int windowSize);

    Ref<Stream> source_;
    Ref<ByteArray> window_;
    const int w_; // window size
    int m_; // window fill
    int i_; // read offset
    bool done_;
};

}} // namespace cc::stream
