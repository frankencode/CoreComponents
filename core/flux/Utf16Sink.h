/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_UTF16SINK_H
#define FLUX_UTF16SINK_H

#include "ByteSink.h"

namespace flux
{

class Utf16EncodeError {};
class Utf16EncodeSurrogatePairError: public Utf16EncodeError {};
class Utf16EncodeByteOrderMarkError: public Utf16EncodeError {};
class Utf16EncodeIllegalCodePointError: public Utf16EncodeError {};

class Utf16Sink: public Object
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
			if ((0xD800 <= ch) && (ch <= 0xDFFF)) throw Utf16EncodeSurrogatePairError();
			else if ((ch == 0xFEFF) || (ch == 0xFFFE)) throw Utf16EncodeByteOrderMarkError();
			byteSink_->writeUInt16(ch);
		}
		else {
			if (0x10FFFF < ch) throw Utf16EncodeIllegalCodePointError();
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

} // namespace flux

#endif // FLUX_UTF16SINK_H
