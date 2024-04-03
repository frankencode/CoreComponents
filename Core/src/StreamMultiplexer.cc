/*
 * Copyright (C) 2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/StreamMultiplexer>

namespace cc {

struct StreamMultiplexer::State final: public Stream::State
{
    State(const List<Stream> &sinks):
        sinks_{sinks}
    {}

    State(std::initializer_list<Stream> sinks):
        sinks_{sinks}
    {}

    bool waitEstablished(int timeout) override
    {
        for (Stream &sink: sinks_) {
            if (!sink.waitEstablished(timeout)) {
                return false;
            }
        }
        return true;
    }

    void write(const Bytes &buffer, long fill) override
    {
        for (Stream &sink: sinks_) {
            sink.write(buffer, fill);
        }
    }

    void write(const List<Bytes> &buffers) override
    {
        for (Stream &sink: sinks_) {
            sink.write(buffers);
        }
    }

    bool isDiscarding() const override
    {
        for (const Stream &sink: sinks_) {
            if (!sink.isDiscarding()) {
                return false;
            }
        }
        return true;
    }

    List<Stream> sinks_;
};

StreamMultiplexer::StreamMultiplexer(const List<Stream> &sinks):
    Stream{new State{sinks}}
{}

StreamMultiplexer::StreamMultiplexer(std::initializer_list<Stream> sinks):
    Stream{new State{sinks}}
{}

} // namespace cc
