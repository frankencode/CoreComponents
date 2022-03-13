/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/TransferMeter>

namespace cc {

struct TransferMeter::State: public Stream::State
{
    State(const Stream &stream):
        stream_{stream}
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
        return n;
    }

    void write(const Bytes &buffer, long fill) override
    {
        totalWritten_ += fill > 0 ? fill : buffer.count();
        stream_.write(buffer, fill);
    }

    void write(const List<Bytes> &buffers) override
    {
        for (const Bytes &buffer: buffers) {
            totalWritten_ += buffer.count();
        }
        stream_.write(buffers);
    }

    Stream stream_;
    long long totalRead_ { 0 };
    long long totalWritten_ { 0 };
};

TransferMeter::TransferMeter(const Stream &stream):
    Stream{new State{stream}}
{}

long long TransferMeter::totalRead() const
{
    return me().totalRead_;
}

long long TransferMeter::totalWritten() const
{
    return me().totalWritten_;
}

const TransferMeter::State &TransferMeter::me() const
{
    return Object::me.as<State>();
}

} // namespace cc
