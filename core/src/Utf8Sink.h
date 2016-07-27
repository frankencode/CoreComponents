/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/ByteSink>
#include <cc/utf8>

namespace cc {

/** \class Utf8Sink Utf8Sink.h cc/Utf8Sink
  * \brief %Unicode UTF-8 encoding data sink
  */
class Utf8Sink: public Object
{
public:
    inline static Ref<Utf8Sink> open(Stream *stream, ByteArray* buf = 0) {
        return new Utf8Sink(stream, buf);
    }

    inline static Ref<Utf8Sink> open(ByteArray* buf) {
        return new Utf8Sink(0, buf);
    }

    void write(uchar_t ch);

    inline ByteSink *byteSink() const { return byteSink_; }

private:
    Utf8Sink(Stream *stream, ByteArray* buf):
        byteSink_(ByteSink::open(stream, buf))
    {}

    Ref<ByteSink> byteSink_;
};

inline void Utf8Sink::write(uchar_t ch)
{
    if (ch < 0x80) { // ASCII range: 0xxxxxxx
        byteSink_->writeUInt8(ch);
    }
    else if (ch < 0x800) { // two-byte codes: 110yyyxx | 10xxxxx
        byteSink_->writeUInt8((ch >> 6) | 0xC0);   // 0xC = (1100)2, code prefix: (110)2
        byteSink_->writeUInt8((ch & 0x3F) | 0x80); // 0x8 = (1000)2, code prefix: (10)2
    }
    else if (ch < 0x10000) { // three-byte codes: 1110yyyy | 10yyyyxx | 10xxxxxx
        if ((0xD800 <= ch) && (ch <= 0xDFFF)) throw utf8::EncodeSurrogatePairError();
        else if ((0xFDD0 <= ch) && (ch <= 0xFDEF)) throw utf8::EncodeNonCharacterError();
        byteSink_->writeUInt8((ch >> 12) | 0xE0);         // 0xE = (1110)2, code prefix: (1110)2
        byteSink_->writeUInt8(((ch >> 6) & 0x3F) | 0x80); // 0x8 = (1000)2, code prefix: (10)2
        byteSink_->writeUInt8((ch & 0x3F) | 0x80);        // 0x8 = (1000)2, code prefix: (10)2
    }
    else if (ch < 0x110000) { // four-byte codes: 11110zzz | 10zzyyyy | 10yyyyxx | 10xxxxxx
        if ((ch & 0xFFFE) == 0xFFFE) throw utf8::EncodeNonCharacterError();
        byteSink_->writeUInt8((ch >> 18) | 0xF0);           // 0xF = (1111)2, code prefix: (11110)2
        byteSink_->writeUInt8(((ch >> 12) & 0x3F) | 0x80); // 0x8 = (1000)2, code prefix: (10)2
        byteSink_->writeUInt8(((ch >> 6) & 0x3F) | 0x80);  // 0x8 = (1000)2, code prefix: (10)2
        byteSink_->writeUInt8((ch & 0x3F) | 0x80);         // 0x8 = (1000)2, code prefix: (10)2
    }
    else {
        throw utf8::EncodeLargeCodePointError();
    }
}

} // namespace cc
