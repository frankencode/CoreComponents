/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/HttpChunkedSink>
#include <cc/Format>

namespace cc {

struct HttpChunkedSink::State: public Stream::State
{
    State(const Stream &stream):
        stream_{stream}
    {}

    ~State()
    {
        Format{stream_} << 0 << "\r\n" << "\r\n";
    }

    long read(Out<Bytes> buffer, long maxFill) override
    {
        return 0;
    }

    void write(const Bytes &buffer, long fill) override
    {
        String hexLength = hex(fill >= 0 ? fill : buffer.count());
        Format{stream_}
            << hexLength
            << "\r\n"
            << (0 <= fill && fill < buffer.count() ? Bytes{buffer}.select(0, fill) : buffer)
            << "\r\n";
    }

    void write(const List<Bytes> &buffers) override
    {
        Format chunk{stream_};
        long total = 0;
        for (const Bytes &buffer: buffers) {
            total += buffer.count();
        }
        String hexLength = hex(total);
        chunk << hexLength << "\r\n";
        for (const Bytes &buffer: buffers) {
            chunk << buffer;
        }
        chunk << "\r\n";
    }

    Stream stream_;
};

HttpChunkedSink::HttpChunkedSink(const Stream &stream):
    Stream{new State{stream}}
{}

} // namespace cc
