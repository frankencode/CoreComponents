/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_UTF16SINK_H
#define FLUX_UTF16SINK_H

#include <flux/ByteSink>
#include <flux/utf16>

namespace flux {

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
			if ((0xD800 <= ch) && (ch <= 0xDFFF)) throw utf16::EncodeSurrogatePairError();
			else if ((ch == 0xFEFF) || (ch == 0xFFFE)) throw utf16::EncodeByteOrderMarkError();
			byteSink_->writeUInt16(ch);
		}
		else {
			if (0x10FFFF < ch) throw utf16::EncodeLargeCodePointError();
			ch -= 0x10000;
			byteSink_->writeUInt16((ch >> 10) + 0xD800);
			byteSink_->writeUInt16((ch & 0x3FF) + 0xDC00);
		}
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
