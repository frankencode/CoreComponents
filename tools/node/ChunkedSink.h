/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Stream>

namespace ccnode {

using namespace cc;

class ChunkedSink: public Stream
{
public:
    static Ref<ChunkedSink> open(Stream *stream);

    void write(const ByteArray *data) override;
    void write(const StringList *parts) override;

private:
    ChunkedSink(Stream *client);
    ~ChunkedSink();
    Ref<Stream> stream_;
};

} // namespace ccnode
