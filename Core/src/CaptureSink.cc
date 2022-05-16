/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/CaptureSink>

namespace cc {

struct CaptureSink::State: public Stream::State
{
    long read(Out<Bytes> buffer, long maxFill) override
    {
        return 0;
    }

    void write(const Bytes &buffer, long fill) override
    {
        parts.append(buffer.copy(0, fill > 0 ? fill : buffer.count()));
    }

    List<Bytes> parts;
};

CaptureSink::CaptureSink():
    Stream{new CaptureSink::State}
{}

List<Bytes> CaptureSink::collect() const
{
    return me().parts;
}

const CaptureSink::State &CaptureSink::me() const
{
    return Object::me.as<State>();
}

} // namespace cc
