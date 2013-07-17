/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FKIT_UTF16SINK_H
#define FKIT_UTF16SINK_H

#include "ByteSink.h"

namespace fkit
{

class Utf16Sink: public Sink<uchar_t>
{
public:
	inline static Ref<Utf16Sink> open(Stream *stream, ByteArray *buf, int endian = BigEndian) {
		return new Utf16Sink(stream, buf, endian);
	}
	inline static Ref<Utf16Sink> open(ByteArray *buf, int endian = BigEndian) {
		return new Utf16Sink(0, buf, endian);
	}

	inline void writeByteOrderMark() {
		byteSink_->writeUInt16(0xFEFF);
	}

	inline void write(uchar_t ch)
	{
		if (ch < 0x10000) {
			if ((0xD800 <= ch) && (ch <= 0xDFFF))
				FKIT_THROW(EncodingException, "UTF-16 disallows encoding of surrogate pairs (0xD800..0xDFFF)");
			else if ((ch == 0xFEFF) || (ch == 0xFFFE))
				FKIT_THROW(EncodingException, "UTF-16 disallows non-characters reserved for BOM (0xFEFF and 0xFFFE)");
			byteSink_->writeUInt16(ch);
		}
		else {
			if (0x10FFFF < ch)
				FKIT_THROW(EncodingException, "UTF-16 disallows encoding of code points above 0x10FFFF");
			ch -= 0x10000;
			byteSink_->writeUInt16((ch >> 10) + 0xD800);
			byteSink_->writeUInt16((ch & 0x3FF) + 0xDC00);
		}
	}

	inline static int encodedSize(uchar_t ch) {
		return 2 * (1 + (0xFFFF < ch));
	}

	inline ByteSink *byteSink() const { return byteSink_; }

private:
	Utf16Sink(Stream *stream, ByteArray* buf, int endian)
		: byteSink_(ByteSink::open(stream, buf, endian))
	{}

	Ref<ByteSink> byteSink_;
};

} // namespace fkit

#endif // FKIT_UTF16SINK_H
