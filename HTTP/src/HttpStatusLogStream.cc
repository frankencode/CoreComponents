/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/HttpStatusLogStream>

namespace cc {

struct HttpStatusLogStream::State: public Stream::State
{
    State(HttpLoggingServiceInstance::State &sink, LoggingLevel level):
        sink_{sink},
        level_{level}
    {}

    void write(const Bytes &buffer, long fill) override
    {
        String message{buffer, 0, fill < 0 ? buffer.count() : fill}; // \todo inefficient
        if (message.endsWith('\n')) {
            sink_.logMessage(message, level_);
        }
    }

    void write(const List<Bytes> &buffers) override
    {
        String message{buffers};
        if (message.endsWith('\n')) {
            sink_.logMessage(message, level_);
        }
    }

    HttpLoggingServiceInstance::State &sink_;
    LoggingLevel level_;
};

HttpStatusLogStream::HttpStatusLogStream(HttpLoggingServiceInstance::State &sink, LoggingLevel level):
    Stream{new State{sink, level}}
{}

} // namespace cc
