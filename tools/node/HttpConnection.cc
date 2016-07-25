/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/LineSource>
#include <cc/stream/TransferLimiter>
#include <cc/stream/StreamTap>
#include <cc/stream/LineBuffer>
#include "exceptions.h"
#include "ErrorLog.h"
#include "TapBuffer.h"
#include "HttpStream.h"
#include "HttpConnection.h"

namespace ccnode {

using namespace cc::stream;

HttpConnection::HttpConnection(Stream *stream):
    httpStream_(HttpStream::open(stream)),
    stream_(httpStream_),
    lineBuffer_(ByteArray::allocate(0x1000))
{}

HttpConnection::~HttpConnection()
{}

void HttpConnection::setupTransferLog(String label)
{
    Ref<Stream> inputBuffer = TapBuffer::open(errorLog()->debugStream(), label + " >> ");
    Ref<Stream> outputBuffer = TapBuffer::open(errorLog()->debugStream(), label + " << ");
    stream_ = StreamTap::open(stream_, inputBuffer, outputBuffer);
}

bool HttpConnection::isPayloadConsumed() const
{
    return httpStream_->isPayloadConsumed();
}

void HttpConnection::readMessage(HttpMessage *message)
{
    httpStream_->nextHeader();

    message->clear();
    message->payload_ = stream_;

    try {
        Ref<TransferLimiter> limiter = TransferLimiter::open(stream_, 0x10000);
        Ref<LineSource> source = LineSource::open(limiter, lineBuffer_);

        readFirstLine(source, message);

        String line;
        String name, value;
        Ref<StringList> multiValue;
        while (source->read(&line)) {
            if (line == "")
                break;
            if (line->at(0) == ' ' || line->at(0) == '\t') {
                if (!multiValue) {
                    multiValue = StringList::create();
                    multiValue->append(value);
                }
                multiValue->append(line->trimInsitu());
                continue;
            }
            if (multiValue) {
                message->establish(name, multiValue->join());
                multiValue = 0;
            }
            int i = line->find(':');
            if (i == line->count()) throw BadRequest();
            name = line->copy(0, i)->trimInsitu();
            value = line->copy(i + 1, line->count())->trimInsitu();
            if (value != "")
                message->establish(name, value);
        }
        if (multiValue)
            message->establish(name, multiValue->join());
    }
    catch (ReadLimitExceeded &) {
        throw BadRequest();
    }

    onHeaderReceived(message);

    if (message->value("Transfer-Encoding") == "chunked") {
        httpStream_->nextChunk();
    }
    else {
        int64_t length = 0;
        String h;
        if (message->lookup("Content-Length", &h)) {
            bool ok = true;
            length = h->toNumber<int64_t>(&ok);
            if (!ok || length < 0) throw BadRequest();
        }
        httpStream_->nextPayload(length);
    }
}

} // namespace ccnode
