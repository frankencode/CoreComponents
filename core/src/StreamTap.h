/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Stream>

namespace cc {

/** \brief Bi-directional traffic redirection and duplication
  */
class StreamTap: public Stream
{
public:
    static Ref<StreamTap> open(Stream *stream, Stream *inputTap, Stream *outputTap);

    inline Stream *stream() const { return stream_; }

    int read(ByteArray *data) override;
    void write(const ByteArray *data) override;
    void write(const StringList *parts) override;

private:
    StreamTap(Stream *stream, Stream *inputTap, Stream *outputTap);

    Ref<Stream> stream_, inputTap_, outputTap_;
};

} // namespace cc
