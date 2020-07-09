/*
 * Copyright (C) 2007-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/http/HttpConnection>
#include <cc/http/TapBuffer>
#include <cc/http/exceptions>
#include <cc/LineSource>
#include <cc/TransferLimiter>
#include <cc/StreamTap>
#include <cc/LineBuffer>

namespace cc {
namespace http {

HttpConnection::HttpConnection(const Stream &stream):
    httpStream_{stream},
    stream_{httpStream_},
    lineBuffer_{CharArray::allocate(0x1000)}
{}

HttpConnection::~HttpConnection()
{}

void HttpConnection::setupTransferLog(const Stream &debugStream, const String &label)
{
    TapBuffer inputBuffer{debugStream, label + " >> "};
    TapBuffer outputBuffer{debugStream, label + " << "};
    stream_ = StreamTap{stream_, inputBuffer, outputBuffer};
}

bool HttpConnection::isPayloadConsumed() const
{
    return httpStream_->isPayloadConsumed();
}

void HttpConnection::readMessage(HttpMessage *message)
{
    httpStream_->nextHeader();

    message->deplete();
    message->payload_ = stream_;

    try {
        TransferLimiter limiter{stream_, 0x10000};
        LineSource source{limiter, mutate(lineBuffer_)};

        readFirstLine(source, message);

        String name, value;
        StringList multiValue;
        for (String line; source->read(&line);) {
            if (line == "")
                break;
            if (line->at(0) == ' ' || line->at(0) == '\t') {
                if (multiValue->count() == 0) {
                    multiValue = StringList{value};
                }
                mutate(line)->trimInsitu();
                multiValue->append(line);
                continue;
            }
            if (multiValue->count() != 0) {
                message->establish(name, multiValue->join());
                multiValue->deplete();
            }
            int i = 0;
            if (!line->find(':', &i)) throw BadRequest{};
            name = line->copy(0, i);
            value = line->copy(i + 1, line->count());
            mutate(name)->trimInsitu();
            mutate(value)->trimInsitu();
            if (value != "")
                message->establish(name, value);
        }
        if (multiValue->count() != 0)
            message->establish(name, multiValue->join());
    }
    catch (ReadLimitExceeded &) {
        throw BadRequest{};
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
            if (!ok || length < 0) throw BadRequest{};
        }
        httpStream_->nextPayload(length);
    }
}

}} // namespace cc::http
