/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Stream>
#include "HttpRequest.h"

namespace ccnode {

using namespace cc;

class HttpStream: public Stream
{
public:
    static Ref<HttpStream> open(Stream *stream);

    bool isPayloadConsumed() const;

    void nextHeader();
    void nextPayload(int64_t length);
    void nextLine();
    void nextChunk();

    int read(ByteArray *data) override;
    void write(const ByteArray *data) override;
    void write(const StringList *parts) override;

private:
    HttpStream(Stream *stream);

    Ref<Stream> stream_;
    Ref<ByteArray> pending_;
    int pendingIndex_;
    int64_t payloadLeft_;
    int nlCount_, nlMax_;
    bool eoi_;
    bool chunked_;
};

} // namespace ccnode
