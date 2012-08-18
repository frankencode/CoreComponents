/*
 * Utf16Encoder.hpp -- UTF-16 encoder
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */
#ifndef FTL_UTF16ENCODER_HPP
#define FTL_UTF16ENCODER_HPP

#include "ByteEncoder.hpp"

namespace ftl
{

class Utf16Encoder: public Sink<uchar_t>
{
public:
	inline static Ref<Utf16Encoder, Owner> newInstance(Ref<Stream> stream, int bufCapa, int endian = BigEndian) {
		return new Utf16Encoder(stream, bufCapa, endian);
	}
	inline static Ref<Utf16Encoder, Owner> newInstance(void *buf, int bufCapa, int endian = BigEndian) {
		return new Utf16Encoder(buf, bufCapa, endian);
	}

	inline void writeByteOrderMark() {
		byteEncoder_->writeUInt16(0xFEFF);
	}

	inline void write(uchar_t ch)
	{
		if (ch < 0x10000) {
			if ((0xD800 <= ch) && (ch <= 0xDFFF))
				FTL_THROW(EncodingException, "UTF-16 disallows encoding of surrogate pairs (0xD800..0xDFFF)");
			else if ((ch == 0xFEFF) || (ch == 0xFFFE))
				FTL_THROW(EncodingException, "UTF-16 disallows non-characters reserved for BOM (0xFEFF and 0xFFFE)");
			byteEncoder_->writeUInt16(ch);
		}
		else {
			if (0x10FFFF < ch)
				FTL_THROW(EncodingException, "UTF-16 disallows encoding of code points above 0x10FFFF");
			ch -= 0x10000;
			byteEncoder_->writeUInt16((ch >> 10) + 0xD800);
			byteEncoder_->writeUInt16((ch & 0x3FF) + 0xDC00);
		}
	}

	inline static int encodedSize(uchar_t ch) {
		return 2 * (1 + (0xFFFF < ch));
	}

	inline Ref<ByteEncoder> byteEncoder() const { return byteEncoder_; }

private:
	Utf16Encoder(Ref<Stream> stream, int bufCapa, int endian)
		: byteEncoder_(ByteEncoder::newInstance(stream, bufCapa, endian))
	{}

	Utf16Encoder(void *buf, int bufCapa, int endian = BigEndian)
		: byteEncoder_(ByteEncoder::newInstance(buf, bufCapa, endian))
	{}

	Ref<ByteEncoder, Owner> byteEncoder_;
};

} // namespace ftl

#endif // FTL_UTF16ENCODER_HPP
