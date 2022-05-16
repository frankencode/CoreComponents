/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/StreamTap>

namespace cc {

struct StreamTap::State: public Stream::State
{
    State(const Stream &stream, const Stream &inputTap, const Stream &outputTap):
        stream_{stream},
        inputTap_{inputTap},
        outputTap_{outputTap}
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
        if (inputTap_) inputTap_.write(buffer, n);
        return n;
    }

    void write(const Bytes &buffer, long fill) override
    {
        stream_.write(buffer, fill);
        if (outputTap_) outputTap_.write(buffer, fill);
    }

    void write(const List<Bytes> &buffers) override
    {
        stream_.write(buffers);
        if (outputTap_) outputTap_.write(buffers);
    }

    bool isDiscarding() const override
    {
        return stream_.isDiscarding();
    }

    Stream stream_;
    Stream inputTap_;
    Stream outputTap_;
};

StreamTap::StreamTap(const Stream &stream, const Stream &inputTap, const Stream &outputTap):
    Stream{new StreamTap::State{stream, inputTap, outputTap}}
{}

} // namespace cc
