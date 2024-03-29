/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/HttpStream>
#include <cc/NullStream>
#include <cc/input>

namespace cc {

struct HttpStream::State: public Stream::State
{
    State(const Stream &stream):
        stream_{stream}
    {}

    bool isPayloadConsumed() const
    {
        return payloadLeft_ == 0;
    }

    void nextHeader()
    {
        if (eoi_) return;
        if (payloadLeft_ > 0) throw HttpCloseRequest{};
        payloadLeft_ = -1;
        nlCount_ = 0;
        nlMax_ = 2;
    }

    void nextPayload(long length)
    {
        if (eoi_) throw HttpCloseRequest{};
        payloadLeft_ = length;
        chunked_ = false;
    }

    void nextLine()
    {
        if (eoi_) throw HttpCloseRequest{};
        payloadLeft_ = -1;
        nlCount_ = 0;
        nlMax_ = 1;
    }

    void nextChunk()
    {
        if (chunked_) {
            chunked_ = false;
            nextLine();
            drain(buffer());
        }
        nextLine();
        String line = readAll(buffer());
        line.trim();
        chunked_ = true;
        if (!scanNumber<long>(line, &payloadLeft_, 16)) {
            throw HttpBadRequest{};
        }
        if (payloadLeft_ == 0) {
            chunked_ = false;
            nextHeader();
            nlCount_ = 1;
            discard();
        }
    }

    bool waitEstablished(int timeout) override
    {
        return stream_.waitEstablished(timeout);
    }

    long read(Out<Bytes> buffer, long maxFill) override
    {
        if (eoi_) return 0;

        if (maxFill < 0) maxFill = buffer().count();
        if (maxFill == 0) return 0;

        if (payloadLeft_ == 0) {
            if (chunked_) {
                nextChunk();
                if (payloadLeft_ == 0)
                    return 0;
            }
            else return 0;
        }

        long n = 0;
        if (pending_) {
            n = pending_.count() - pendingIndex_;
            if (n > maxFill) n = maxFill;
            pending_.copyRangeTo(pendingIndex_, pendingIndex_ + n, &buffer);
            pendingIndex_ += n;
            if (pendingIndex_ == pending_.count()) pending_ = Bytes{};
        }
        else {
            n = stream_.read(buffer);
        }

        if (n == 0) {
            eoi_ = true;
            return 0;
        }

        if (payloadLeft_ == -1) {
            int i = 0;
            for (;i < n && nlCount_ < nlMax_; ++i) {
                if (buffer().at(i) == '\r')
                    continue;
                if (buffer().at(i) == '\n') {
                    ++nlCount_;
                    continue;
                }
                nlCount_ = 0;
            }
            if (nlCount_ == nlMax_) {
                if (i < n) pending_ = buffer().select(i, n);
                pendingIndex_ = 0;
                payloadLeft_ = 0;
                n = i;
            }
        }
        else if (payloadLeft_ < n) {
            pending_ = buffer().select(payloadLeft_, n);
            pendingIndex_ = 0;
            n = payloadLeft_;
            payloadLeft_ = 0;
        }
        else {
            payloadLeft_ -= n;
        }

        return n;
    }

    void write(const Bytes &buffer, long fill) override
    {
        stream_.write(buffer, fill);
    }

    void write(const List<Bytes> &buffers) override
    {
        stream_.write(buffers);
    }

    void write(const struct iovec *iov, int iovcnt) override
    {
        stream_.write(iov, iovcnt);
    }

    Bytes &buffer()
    {
        if (!buffer_) buffer_ = Bytes::allocate(0x1000);
        return buffer_;
    }

    void discard()
    {
        transferTo(drainage_, 0x10000, buffer());
    }

    Stream stream_;
    NullStream drainage_;
    Bytes buffer_;
    Bytes pending_;
    int pendingIndex_ { 0 };
    long payloadLeft_ { -1 };
    int nlCount_ { 0 };
    int nlMax_ { 0 };
    bool eoi_ { false };
    bool chunked_ { false };
};

HttpStream::HttpStream(const Stream &stream):
    Stream{new State{stream}}
{}

bool HttpStream::isPayloadConsumed() const
{
    return me().isPayloadConsumed();
}

void HttpStream::nextHeader()
{
    me().nextHeader();
}

void HttpStream::nextPayload(long length)
{
    me().nextPayload(length);
}

void HttpStream::nextLine()
{
    me().nextLine();
}

void HttpStream::nextChunk()
{
    me().nextChunk();
}

HttpStream::State &HttpStream::me()
{
    return Object::me.as<State>();
}

const HttpStream::State &HttpStream::me() const
{
    return Object::me.as<State>();
}

} // namespace cc
