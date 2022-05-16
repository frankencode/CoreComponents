/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/TransferLimiter>

namespace cc {

struct TransferLimiter::State: public Stream::State
{
    State(const Stream &stream, long long readLimit, long long writeLimit):
        stream_{stream},
        readLimit_{readLimit},
        writeLimit_{writeLimit}
    {}

    bool waitEstablished(int timeout) override
    {
        return stream_.waitEstablished(timeout);
    }

    bool wait(IoEvent event, int timeout) override
    {
        return stream_.wait(event, timeout);
    }

    long read(Out<Bytes> buffer, long maxFill) override
    {
        long n = stream_.read(buffer, maxFill);
        totalRead_ += n;
        if (readLimit_ > 0 && totalRead_ > readLimit_) {
            throw InputExhaustion{};
        }
        return n;
    }

    void write(const Bytes &buffer, long fill) override
    {
        totalWritten_ += fill >= 0 ? fill : buffer.count();
        if (writeLimit_ > 0 && totalWritten_ > writeLimit_) {
            throw OutputExhaustion{};
        }
        stream_.write(buffer, fill);
    }

    void write(const List<Bytes> &buffers) override
    {
        for (const Bytes &buffer: buffers) {
            totalWritten_ += buffer.count();
        }
        if (writeLimit_ > 0 && totalWritten_ > writeLimit_) {
            throw OutputExhaustion{};
        }
        stream_.write(buffers);
    }

    Stream stream_;
    long long readLimit_ { -1 };
    long long writeLimit_ { -1 };
    long long totalRead_ { 0 };
    long long totalWritten_ { 0 };
};

TransferLimiter::TransferLimiter(const Stream &stream, long long readLimit, long long writeLimit):
    Stream{new TransferLimiter::State{stream, readLimit, writeLimit}}
{}

TransferLimiter::TransferLimiter(State *newState):
    Stream{newState}
{}

long long TransferLimiter::readLimit() const
{
    return me().readLimit_;
}

long long TransferLimiter::writeLimit() const
{
    return me().writeLimit_;
}

long long TransferLimiter::totalRead() const
{
    return me().totalRead_;
}

long long TransferLimiter::totalWritten() const
{
    return me().totalWritten_;
}

void TransferLimiter::reset()
{
    me().totalRead_ = 0;
    me().totalWritten_ = 0;
}

const TransferLimiter::State &TransferLimiter::me() const
{
    return Object::me.as<State>();
}

TransferLimiter::State &TransferLimiter::me()
{
    return Object::me.as<State>();
}

} // namespace cc
