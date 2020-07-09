/*
 * Copyright (C) 2007-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/http/HttpChunkedSink>
#include <cc/Format>

namespace cc {
namespace http {

HttpChunkedSink::Instance::Instance(const Stream &stream):
    stream_{stream}
{}

HttpChunkedSink::Instance::~Instance()
{
    Format{stream_} << 0 << "\r\n" << "\r\n";
}

void HttpChunkedSink::Instance::write(const CharArray *data)
{
    Format{stream_} << hex(data->count()) << "\r\n" << data << "\r\n";
}

void HttpChunkedSink::Instance::write(const StringList &parts)
{
    Format chunk{stream_};
    int total = 0;
    for (int i = 0; i < parts->count(); ++i)
        total += parts->at(i)->count();
    chunk << hex(total) << "\r\n";
    for (int i = 0; i < parts->count(); ++i)
        chunk << parts->at(i);
    chunk << "\r\n";
}

}} // namespace cc::http
