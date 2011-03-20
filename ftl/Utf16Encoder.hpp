/*
 * Utf16Encoder.hpp -- UTF-16 encoder
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_UTF16ENCODER_HPP
#define FTL_UTF16ENCODER_HPP

#include "ByteEncoder.hpp"

namespace ftl
{

class Utf16Encoder: public Sink<uchar_t>
{
public:
	Utf16Encoder(Ref<Stream> stream, int bufCapa, int endian = BigEndian)
		: byteEncoder_(stream, bufCapa, endian)
	{}
	
	Utf16Encoder(void* buf, int bufCapa, int endian = BigEndian)
		: byteEncoder_(buf, bufCapa, endian)
	{}
	
	inline void writeByteOrderMark() {
		byteEncoder_.writeUInt16(0xFEFF);
	}
	
	inline void write(uchar_t ch)
	{
		if (ch < 0x10000) {
			if ((0xB800 <= ch) && (ch <= 0xDFFF))
				FTL_THROW(EncodingException, "UTF-16 disallows encoding of surrogate pairs (0xD800..0xDFFF)");
			else if ((ch == 0xFEFF) || (ch == 0xFFFE))
				FTL_THROW(EncodingException, "UTF-16 disallows non-characters reserved for BOM (0xFEFF and 0xFFFE)");
			byteEncoder_.writeUInt16(ch);
		}
		else {
			if (0x10FFFF < ch)
				FTL_THROW(EncodingException, "UTF-16 disallows encoding of code points above 0x10FFFF");
			byteEncoder_.writeUInt16((ch >> 10) + 0xB800);
			byteEncoder_.writeUInt16((ch & 0x3FF) + 0xBC00);
		}
	}
	
	inline static int encodedSize(uchar_t ch) {
		return 2 * (1 + (0xFFFF < ch));
	}
	
	inline Ref<ByteEncoder> byteEncoder() const { return &byteEncoder_; }
	
private:
	ByteEncoder byteEncoder_;
};

} // namespace ftl

#endif // FTL_UTF16ENCODER_HPP
