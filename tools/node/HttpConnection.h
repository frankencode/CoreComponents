/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/net/StreamSocket>
#include "HttpMessage.h"

namespace cc { class LineSource; }

namespace ccnode {

using namespace cc;
using namespace cc::net;

class ServiceWorker;
class HttpStream;

class HttpConnection: public Object
{
public:
    inline Stream *stream() const { return stream_; }

    void setupTransferLog(String label);

    bool isPayloadConsumed() const;

protected:
    HttpConnection(Stream *stream);
    virtual ~HttpConnection();

    void readMessage(HttpMessage *message);
    virtual void readFirstLine(LineSource *source, HttpMessage *message) {}
    virtual void onHeaderReceived(HttpMessage *message) {}

    Ref<HttpStream> httpStream_;
    Ref<Stream> stream_;
    Ref<ByteArray> lineBuffer_;
};

} // namespace ccnode
