/*
 * Utf8Encoder.hpp -- UTF-8 encoder
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */
#ifndef FTL_UTF8ENCODER_HPP
#define FTL_UTF8ENCODER_HPP

#include "ByteEncoder.hpp"

namespace ftl
{

class Utf8Encoder: public Sink<uchar_t>
{
public:
	inline static Ref<Utf8Encoder, Owner> newInstance(Ref<Stream> stream, int bufCapa = FTL_DEFAULT_BUF_CAPA) {
		return new Utf8Encoder(stream, bufCapa);
	}

	inline static Ref<Utf8Encoder, Owner> newInstance(void *buf, int bufCapa) {
		return new Utf8Encoder(buf, bufCapa);
	}

	void write(uchar_t ch);
	static int encodedSize(uchar_t ch);

	inline Ref<ByteEncoder> byteEncoder() const { return byteEncoder_; }

private:
	Utf8Encoder(Ref<Stream> stream, int bufCapa)
		: byteEncoder_(ByteEncoder::newInstance(stream, bufCapa))
	{}

	Utf8Encoder(void *buf, int bufCapa)
		: byteEncoder_(ByteEncoder::newInstance(buf, bufCapa))
	{}

	Ref<ByteEncoder, Owner> byteEncoder_;
};

inline void Utf8Encoder::write(uchar_t ch)
{
	if (ch < 0x80) { // ASCII range: 0xxxxxxx
		byteEncoder_->writeUInt8(ch);
	}
	else if (ch < 0x800) { // two-byte codes: 110yyyxx | 10xxxxx
		byteEncoder_->writeUInt8((ch >> 6) | 0xC0);   // 0xC = (1100)2, code prefix: (110)2
		byteEncoder_->writeUInt8((ch & 0x3F) | 0x80); // 0x8 = (1000)2, code prefix: (10)2
	}
	else if (ch < 0x10000) { // three-byte codes: 1110yyyy | 10yyyyxx | 10xxxxxx
		if ((0xD800 <= ch) && (ch <= 0xDFFF))
			FTL_THROW(EncodingException, "UTF-8 disallows encoding of UTF-16 surrogate pairs 0xD800..0xDFFF");
		else if ((0xFDD0 <= ch) && (ch <= 0xFDEF))
			FTL_THROW(EncodingException, "UTF-8 disallows encoding of non-characters 0xFDD0..0xFDEF");
		byteEncoder_->writeUInt8((ch >> 12) | 0xE0);         // 0xE = (1110)2, code prefix: (1110)2
		byteEncoder_->writeUInt8(((ch >> 6) & 0x3F) | 0x80); // 0x8 = (1000)2, code prefix: (10)2
		byteEncoder_->writeUInt8((ch & 0x3F) | 0x80);        // 0x8 = (1000)2, code prefix: (10)2
	}
	else if (ch < 0x110000) { // four-byte codes: 11110zzz | 10zzyyyy | 10yyyyxx | 10xxxxxx
		if ((ch & 0xFFFE) == 0xFFFE)
			FTL_THROW(EncodingException, "UTF-8 disallows encoding of non-characters 0x??FFFE,0x??FFFF");
		byteEncoder_->writeUInt8((ch >> 18) | 0xF0);           // 0xF = (1111)2, code prefix: (11110)2
		byteEncoder_->writeUInt8(((ch >> 12) & 0x3F) | 0x80); // 0x8 = (1000)2, code prefix: (10)2
		byteEncoder_->writeUInt8(((ch >> 6) & 0x3F) | 0x80);  // 0x8 = (1000)2, code prefix: (10)2
		byteEncoder_->writeUInt8((ch & 0x3F) | 0x80);         // 0x8 = (1000)2, code prefix: (10)2
	}
	else {
		FTL_THROW(EncodingException, "UTF-8 disallows encoding of code points above 0x10FFFF");
	}
}

inline int Utf8Encoder::encodedSize(uchar_t ch)
{
	int n = 0;
	if (ch < 0x80)         n = 1; // ASCII range: 0xxxxxxx
	else if (ch < 0x800)   n = 2; // two-byte codes: 110yyyxx | 10xxxxx
	else if (ch < 0x10000) n = 3; // three-byte codes: 1110yyyy | 10yyyyxx | 10xxxxxx
	else if (ch < 0x11000) n = 4; // four-byte codes: 11110zzz | 10zzyyyy | 10yyyyxx | 10xxxxxx
	return n;
}

} // namespace ftl

#endif // FTL_UTF8ENCODER_HPP
