/*
 * Copyright (C) 2007-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Format>
#include <cc/TransferMeter>
#include <cc/node/HttpConnection>
#include <cc/node/HttpChunkedSink>
#include <cc/node/HttpGenerator>

namespace cc {
namespace node {

HttpGenerator::HttpGenerator(HttpConnection *peer):
    peer_{peer},
    header_{Header::create()},
    headerWritten_{false},
    contentLength_{-1},
    bytesWritten_{0}
{}

HttpGenerator::~HttpGenerator()
{}

void HttpGenerator::setHeader(const String &name, const String &value)
{
    header_->establish(name, value);
}

void HttpGenerator::setHeader(Header *header)
{
    Ref<Header> headerSaved = header_;
    header_ = header;
    for (int i = 0; i < headerSaved->count(); ++i)
        header_->insert(headerSaved->keyAt(i), headerSaved->valueAt(i));
}

void HttpGenerator::writeHeader()
{
    polishHeader();

    Format sink(peer_->stream());
    writeFirstLine(sink);
    for (int i = 0; i < header_->count(); ++i)
        sink << header_->keyAt(i) << ":" << header_->valueAt(i) << "\r\n";
    sink << "\r\n";

    headerWritten_ = true;
}

void HttpGenerator::beginTransmission(ssize_t contentLength)
{
    if (!headerWritten_) {
        contentLength_ = contentLength;
        writeHeader();
    }
}

Stream *HttpGenerator::payload()
{
    if (!payload_) {
        if (!headerWritten_) writeHeader();
        Ref<Stream> stream = peer_->stream();
        if (contentLength_ < 0) {
            stream = HttpChunkedSink::open(stream);
        }
        payload_ = TransferMeter::open(stream);
    }
    return payload_;
}

void HttpGenerator::write(const String &bytes)
{
    payload()->write(bytes);
}

Format HttpGenerator::chunk(const String &pattern)
{
    return Format(pattern, payload());
}

Format HttpGenerator::chunk()
{
    return Format(payload());
}

void HttpGenerator::endTransmission()
{
    if (payload_) {
        bytesWritten_ = payload_->totalWritten();
        payload_ = 0;
    }
}

void HttpGenerator::transmit()
{
    beginTransmission();
    endTransmission();
}

}} // namespace cc::node
