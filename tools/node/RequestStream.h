/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXNODE_REQUESTSTREAM_H
#define FLUXNODE_REQUESTSTREAM_H

#include <flux/Stream>
#include "Request.h"

namespace fluxnode
{

using namespace flux;

class RequestStream: public Stream
{
public:
    static Ref<RequestStream> open(Stream *stream);

    void setupTimeout(double interval);
    bool isPayloadConsumed() const;

    void nextHeader();
    void nextPayload(int64_t length);
    void nextLine();
    void nextChunk();

    virtual bool readyRead(double interval) const;
    virtual int read(ByteArray *buf);

    virtual void write(const ByteArray *buf);
    virtual void write(const StringList *parts);

private:
    RequestStream(Stream *stream);

    Ref<Stream> stream_;
    Ref<ByteArray> pending_;
    int pendingIndex_;
    int64_t bytesLeft_;
    int nlCount_, nlMax_;
    bool eoi_;
    bool chunked_;
};

} // namespace fluxnode

#endif // FLUXNODE_REQUESTSTREAM_H
