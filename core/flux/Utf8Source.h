/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_UTF8SOURCE_H
#define FLUX_UTF8SOURCE_H

#include "ByteSource.h"
#include "utf8.h"

namespace flux
{

class Utf8Source: public Object
{
public:
	inline static Ref<Utf8Source> open(Stream *stream, ByteArray *buf = 0) {
		return new Utf8Source(stream, buf);
	}
	inline static Ref<Utf8Source> open(ByteArray *buf) {
		return new Utf8Source(0, buf);
	}

	inline bool read(uchar_t *ch)
	{
		bool more = byteSource_->hasMore();
		if (more) {
			*ch = byteSource_->readUInt8();
			if ((*ch & 0x80) != 0) // distinguish 7 bit ASCII from multibyte sequence
				*ch = readMultiByte(*ch);
		}
		return more;
	}

	inline ByteSource *byteSource() const { return byteSource_; }

private:
	Utf8Source(Stream *stream, ByteArray *buf)
		: byteSource_(ByteSource::open(stream, buf))
	{}

	uchar_t readMultiByte(uchar_t ch);

	Ref<ByteSource> byteSource_;
};

inline uchar_t Utf8Source::readMultiByte(uchar_t ch)
{
	int n = -1; // number of additional bytes

	// translate prefix to code length (n is the number of successive bytes)
	if ((ch >> 5) == 6) { // code prefix: 6 = (110)2
		ch = ch & 0x1F;
		n = 1;
	}
	else if ((ch >> 4) == 14) { // code prefix: 14 = (1110)2
		ch = ch & 0x0F;
		n = 2;
	}
	else if ((ch >> 3) == 30) { // code prefix: 30 = (11110)2
		ch = ch & 0x07;
		n = 3;
	}

	int n2 = n;

	// read n successive characters (chs), which carry the code prefix (10)2
	while (n > 0) {
		uint8_t chs = byteSource_->readUInt8();
		if ((chs >> 6) == 2) // 2 = (10)2
			ch = (ch << 6) | (chs & 0x3F);
		else
			break;
		--n;
	}

	// enforce minimum code length
	if (n == 0) {
		if (n2 == 1) {
			if (ch < 0x80) n = -1;
		}
		else if (n2 == 2) {
			if (ch < 0x800) n = -1;
		}
		else if (n2 == 3) {
			if (ch < 0x10000) n = -1;
		}
	}

	if (n < 0) throw utf8::DecodeError();

	return ch;
}

} // namespace flux

#endif // FLUX_UTF8SOURCE_H
