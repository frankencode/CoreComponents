/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/HttpMessageGenerator>
#include <cc/HttpChunkedSink>

namespace cc {

void HttpMessageGenerator::State::setHeader(const String &name, const String &value)
{
    header_.establish(name, value);
}

void HttpMessageGenerator::State::transmit()
{
    beginTransmission(0);
    endTransmission();
}

void HttpMessageGenerator::State::transmit(const String &payload)
{
    beginTransmission(payload.count());
    if (payload.count() > 0)
        write(payload);
    endTransmission();
}

void HttpMessageGenerator::State::transmit(const Stream &source)
{
    beginTransmission(-1);
    Stream{source}.transferTo(payload());
    endTransmission();
}

void HttpMessageGenerator::State::setHeader(const Map<String, String> &header)
{
    Map<String, String> headerSaved = header_;
    header_ = header;
    for (const auto &item: headerSaved)
        header_.insert(item.key(), item.value());
}

void HttpMessageGenerator::State::writeHeader()
{
    polishHeader();

    Format sink{stream_};
    writeFirstLine(sink);
    for (const auto &item: header_)
        sink << item.key() << ":" << item.value() << "\r\n";
    sink << "\r\n";

    headerWritten_ = true;
}

void HttpMessageGenerator::State::beginTransmission(long long contentLength)
{
    if (!headerWritten_) {
        contentLength_ = contentLength;
        writeHeader();
    }
}

Stream HttpMessageGenerator::State::payload()
{
    if (!payload_) {
        if (!headerWritten_) writeHeader();
        Stream stream = stream_;
        if (contentLength_ < 0) {
            stream = HttpChunkedSink{stream};
        }
        payload_ = TransferMeter{stream};
    }
    return payload_;
}

void HttpMessageGenerator::State::write(const Bytes &data)
{
    payload().write(data);
}

Format HttpMessageGenerator::State::chunk(const String &pattern)
{
    return Format{pattern, payload()};
}

Format HttpMessageGenerator::State::chunk()
{
    return Format{payload()};
}

void HttpMessageGenerator::State::endTransmission()
{
    if (payload_) {
        bytesWritten_ = payload_.totalWritten();
        payload_ = TransferMeter{};
    }
}

} // namespaec cc
