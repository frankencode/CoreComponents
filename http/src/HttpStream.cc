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
    return new HttpStream{stream};
}

HttpStream::HttpStream(Stream *stream):
    stream_{stream},
    payloadLeft_{-1},
    nlCount_{0},
    eoi_{false},
    chunked_{false}
{}

bool HttpStream::isPayloadConsumed() const
{
    return payloadLeft_ == 0;
}

void HttpStream::nextHeader()
{
    if (eoi_) return;
    if (payloadLeft_ > 0) throw CloseRequest{};
    payloadLeft_ = -1;
    nlCount_ = 0;
    nlMax_ = 2;
}

void HttpStream::nextPayload(int64_t length)
{
    if (eoi_) throw CloseRequest{};
    payloadLeft_ = length;
    chunked_ = false;
}

void HttpStream::nextLine()
{
    if (eoi_) throw CloseRequest{};
    payloadLeft_ = -1;
    nlCount_ = 0;
    nlMax_ = 1;
}

void HttpStream::nextChunk()
{
    if (chunked_) {
        chunked_ = false;
        nextLine();
        readAll();
    }
    bool ok = false;
    nextLine();
    payloadLeft_ = readAll()->trim()->toNumber<int, 16>(&ok);
    chunked_ = true;
    if (!ok) throw BadRequest{};
    if (payloadLeft_ == 0) {
        chunked_ = false;
        nextHeader();
        nlCount_ = 1;
        TransferLimiter::open(this, 0x10000)->drain();
    }
}

int HttpStream::read(CharArray *data)
{
    if (eoi_) return 0;

    if (payloadLeft_ == 0) {
        if (chunked_) {
            nextChunk();
            if (payloadLeft_ == 0)
                return 0;
        }
        else return 0;
    }

    int n = 0;
    if (pending_) {
        data->write(pending_->select(pendingIndex_, pending_->count()));
        int h = pending_->count() - pendingIndex_;
        if (data->count() < h) {
            pendingIndex_ += data->count();
            n = data->count();
        }
        else {
            pending_ = nullptr;
            n = h;
        }
    }
    else
        n = stream_->read(data);

    if (n == 0) {
        eoi_ = true;
        return 0;
    }

    if (payloadLeft_ == -1) {
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
    }
    else {
        payloadLeft_ -= n;
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
