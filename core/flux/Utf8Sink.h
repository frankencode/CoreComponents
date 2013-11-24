/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_UTF8SINK_H
#define FLUX_UTF8SINK_H

#include "ByteSink.h"

namespace flux
{

class Utf8EncodeError {};
class Utf8EncodeSurrogatePairError   : public Utf8EncodeError {};
class Utf8EncodeNonCharacterError    : public Utf8EncodeError {};
class Utf8EncodeIllegalCodePointError: public Utf8EncodeError {};

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
	static int encodedSize(uchar_t ch);

	inline ByteSink *byteSink() const { return byteSink_; }

private:
	Utf8Sink(Stream *stream, ByteArray* buf)
		: byteSink_(ByteSink::open(stream, buf))
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
		if ((0xD800 <= ch) && (ch <= 0xDFFF)) throw Utf8EncodeSurrogatePairError();
		else if ((0xFDD0 <= ch) && (ch <= 0xFDEF)) throw Utf8EncodeNonCharacterError();
		byteSink_->writeUInt8((ch >> 12) | 0xE0);         // 0xE = (1110)2, code prefix: (1110)2
		byteSink_->writeUInt8(((ch >> 6) & 0x3F) | 0x80); // 0x8 = (1000)2, code prefix: (10)2
		byteSink_->writeUInt8((ch & 0x3F) | 0x80);        // 0x8 = (1000)2, code prefix: (10)2
	}
	else if (ch < 0x110000) { // four-byte codes: 11110zzz | 10zzyyyy | 10yyyyxx | 10xxxxxx
		if ((ch & 0xFFFE) == 0xFFFE) throw Utf8EncodeNonCharacterError();
		byteSink_->writeUInt8((ch >> 18) | 0xF0);           // 0xF = (1111)2, code prefix: (11110)2
		byteSink_->writeUInt8(((ch >> 12) & 0x3F) | 0x80); // 0x8 = (1000)2, code prefix: (10)2
		byteSink_->writeUInt8(((ch >> 6) & 0x3F) | 0x80);  // 0x8 = (1000)2, code prefix: (10)2
		byteSink_->writeUInt8((ch & 0x3F) | 0x80);         // 0x8 = (1000)2, code prefix: (10)2
	}
	else {
		throw Utf8EncodeIllegalCodePointError();
	}
}

inline int Utf8Sink::encodedSize(uchar_t ch)
{
	int n = 0;
	if (ch < 0x80)         n = 1; // ASCII range: 0xxxxxxx
	else if (ch < 0x800)   n = 2; // two-byte codes: 110yyyxx | 10xxxxx
	else if (ch < 0x10000) n = 3; // three-byte codes: 1110yyyy | 10yyyyxx | 10xxxxxx
	else if (ch < 0x11000) n = 4; // four-byte codes: 11110zzz | 10zzyyyy | 10yyyyxx | 10xxxxxx
	return n;
}

} // namespace flux

#endif // FLUX_UTF8SINK_H
