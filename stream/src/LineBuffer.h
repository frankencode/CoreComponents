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

/** \brief Line output buffer
  */
class LineBuffer: public Stream
{
public:
    static Ref<LineBuffer> open(Stream *stream, String prefix = "");

    inline Stream *stream() const { return stream_; }
    virtual String prefix() const;

    int read(ByteArray *data) override;
    void write(const ByteArray *data) override;
    void write(const StringList *parts) override;

    int flush();

protected:
    LineBuffer(Stream *stream, String prefix = "");

private:
    Ref<Stream> stream_;
    String prefix_;
    Ref<StringList> backlog_;
};

}} // namespace cc::stream
