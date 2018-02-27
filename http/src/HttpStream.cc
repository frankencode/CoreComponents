/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/System>
#include <cc/TransferLimiter>
#include <cc/http/exceptions>
#include <cc/http/HttpStream>

namespace cc {
namespace http {

Ref<HttpStream> HttpStream::open(Stream *stream)
{
    return new HttpStream(stream);
}

HttpStream::HttpStream(Stream *stream):
    stream_(stream),
    payloadLeft_(-1),
    nlCount_(0),
    eoi_(false),
    chunked_(false)
{}

bool HttpStream::isPayloadConsumed() const
{
    return payloadLeft_ == 0;
}

void HttpStream::nextHeader()
{
    if (eoi_) throw CloseRequest();
    if (payloadLeft_ > 0) throw CloseRequest();
    payloadLeft_ = -1;
    nlCount_ = 0;
    nlMax_ = 2;
}

void HttpStream::nextPayload(int64_t length)
{
    if (eoi_) throw CloseRequest();
    payloadLeft_ = length;
    chunked_ = false;
}

void HttpStream::nextLine()
{
    if (eoi_) throw CloseRequest();
    payloadLeft_ = -1;
    nlCount_ = 0;
    nlMax_ = 1;
}

void HttpStream::nextChunk()
{
    nextLine();
    String line = readAll();
    payloadLeft_ = 0;
    for (int i = 0; i < line->count(); ++i) {
        char ch = line->at(i);
        if (ch == '\r' || ch == '\n') continue;
        payloadLeft_ *= 16;
        if ('0' <= ch && ch <= '9') payloadLeft_ += ch - '9';
        else if ('a' <= ch && ch <= 'f') payloadLeft_ += ch - 'a';
        else if ('A' <= ch && ch <= 'F') payloadLeft_ += ch - 'A';
        else throw BadRequest();
    }
    if (payloadLeft_ == 0) {
        chunked_ = false;
        nextHeader();
        nlCount_ = 1;
        TransferLimiter::open(this, 0x10000)->drain();
    }
    else {
        chunked_ = true;
    }
}

int HttpStream::read(CharArray *data)
{
    if (eoi_ || payloadLeft_ == 0)
        return 0;

    int n = 0;
    if (pending_) {
        data->write(pending_->select(pendingIndex_, pending_->count()));
        int h = pending_->count() - pendingIndex_;
        if (data->count() < h) {
            pendingIndex_ += data->count();
            n = data->count();
        }
        else {
            pending_ = 0;
            n = h;
        }
    }
    else
        n = stream_->read(data);

    if (n == 0) {
        eoi_ = true;
    }
    else if (payloadLeft_ == -1) {
        int i = 0;
        for (;i < n && nlCount_ < nlMax_; ++i) {
            if (data->at(i) == '\r')
                continue;
            if (data->at(i) == '\n') {
                ++nlCount_;
                continue;
            }
            nlCount_ = 0;
        }
        if (nlCount_ == nlMax_) {
            if (i < n) pending_ = data->select(i, n);
            pendingIndex_ = 0;
            payloadLeft_ = 0;
            n = i;
        }
    }
    else if (payloadLeft_ < n) {
        pending_ = data->select(payloadLeft_, n);
        pendingIndex_ = 0;
        n = payloadLeft_;
        payloadLeft_ = 0;
        if (chunked_) nextChunk();
    }

    return n;
}

void HttpStream::write(const CharArray *data)
{
    stream_->write(data);
}

void HttpStream::write(const StringList *parts)
{
    stream_->write(parts);
}

}} // namespace cc::http
