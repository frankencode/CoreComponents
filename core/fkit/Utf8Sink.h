/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FKIT_UTF8SINK_H
#define FKIT_UTF8SINK_H

#include "ByteSink.h"

namespace fkit
{

class Utf8Sink: public Sink<uchar_t>
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
		if ((0xD800 <= ch) && (ch <= 0xDFFF))
			FKIT_THROW(EncodingException, "UTF-8 disallows encoding of UTF-16 surrogate pairs 0xD800..0xDFFF");
		else if ((0xFDD0 <= ch) && (ch <= 0xFDEF))
			FKIT_THROW(EncodingException, "UTF-8 disallows encoding of non-characters 0xFDD0..0xFDEF");
		byteSink_->writeUInt8((ch >> 12) | 0xE0);         // 0xE = (1110)2, code prefix: (1110)2
		byteSink_->writeUInt8(((ch >> 6) & 0x3F) | 0x80); // 0x8 = (1000)2, code prefix: (10)2
		byteSink_->writeUInt8((ch & 0x3F) | 0x80);        // 0x8 = (1000)2, code prefix: (10)2
	}
	else if (ch < 0x110000) { // four-byte codes: 11110zzz | 10zzyyyy | 10yyyyxx | 10xxxxxx
		if ((ch & 0xFFFE) == 0xFFFE)
			FKIT_THROW(EncodingException, "UTF-8 disallows encoding of non-characters 0x??FFFE,0x??FFFF");
		byteSink_->writeUInt8((ch >> 18) | 0xF0);           // 0xF = (1111)2, code prefix: (11110)2
		byteSink_->writeUInt8(((ch >> 12) & 0x3F) | 0x80); // 0x8 = (1000)2, code prefix: (10)2
		byteSink_->writeUInt8(((ch >> 6) & 0x3F) | 0x80);  // 0x8 = (1000)2, code prefix: (10)2
		byteSink_->writeUInt8((ch & 0x3F) | 0x80);         // 0x8 = (1000)2, code prefix: (10)2
	}
	else {
		FKIT_THROW(EncodingException, "UTF-8 disallows encoding of code points above 0x10FFFF");
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

} // namespace fkit

#endif // FKIT_UTF8SINK_H
