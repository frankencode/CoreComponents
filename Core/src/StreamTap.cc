/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/StreamTap>

namespace cc {

struct StreamTap::State final: public Stream::State
{
    State(const Stream &stream, const Stream &inboundTap, const Stream &outboundTap):
        stream_{stream},
        inboundTap_{inboundTap},
        outboundTap_{outboundTap}
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
        if (inboundTap_) inboundTap_.write(buffer, n);
        return n;
    }

    void write(const Bytes &buffer, long fill) override
    {
        stream_.write(buffer, fill);
        if (outboundTap_) outboundTap_.write(buffer, fill);
    }

    void write(const List<Bytes> &buffers) override
    {
        stream_.write(buffers);
        if (outboundTap_) outboundTap_.write(buffers);
    }

    bool isDiscarding() const override
    {
        return stream_.isDiscarding();
    }

    Stream stream_;
    Stream inboundTap_;
    Stream outboundTap_;
};

StreamTap::StreamTap(const Stream &stream, const Stream &inboundTap, const Stream &outboundTap):
    Stream{new State{stream, inboundTap, outboundTap}}
{}

Stream StreamTap::stream() const
{
    return me().stream_;
}

StreamTap::State &StreamTap::me()
{
    return Object::me.as<State>();
}

const StreamTap::State &StreamTap::me() const
{
    return Object::me.as<State>();
}

} // namespace cc
