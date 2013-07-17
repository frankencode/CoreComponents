/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FKIT_UTF16SOURCE_H
#define FKIT_UTF16SOURCE_H

#include "ByteSource.h"

namespace fkit
{

/** \brief UTF-16 decoder
  *
  * A Byte-Order-Mark (BOM) at the start of the stream automatically sets
  * endianess. The decoder is designed to be as strict as possible. It checks
  * if the input streams delivers full 16-bit words and it requires high
  * surrogates to be followed by low surrogates.
  *
  * A 7-bit ASCII text is be decoded as UTF-16 without error if stream length is
  * a multiple of 2. Means there is a 50% chance of reading an 7-bit ASCII text
  * as UTF-16 without throwing an EncodingException.
  */
class Utf16Source: public Source<uchar_t>
{
public:
	inline static Ref<Utf16Source> open(Stream *stream, ByteArray *buf, int endian = BigEndian) {
		return new Utf16Source(stream, buf, endian);
	}
	inline static Ref<Utf16Source> open(ByteArray *buf, int endian = BigEndian) {
		return new Utf16Source(0, buf, endian);
	}

	inline bool read(uchar_t *ch)
	{
		bool more = byteSource_->hasMore();
		if (more) {
			uint16_t w = byteSource_->readUInt16();
			if (w == 0xFFFE && firstTime_) {
				firstTime_ = false;
				byteSource_->setEndian((byteSource_->endian() == BigEndian) ? LittleEndian : BigEndian);
				w = byteSource_->readUInt16();
			}
			*ch = w;
			if ((0xD800 <= *ch) && (*ch < 0xDC00)) {
				uint16_t w = byteSource_->readUInt16();
				if (!((0xDC00 <= w) && (w < 0xE000)))
					FKIT_THROW(EncodingException, "Input data is not conforming to UTF-16 encoding");
				*ch = 0x10000 + (((*ch - 0xD800) << 10) | (w - 0xDC00));
			}
			else if (0x10FFFF < *ch) {
				FKIT_THROW(EncodingException, "Input data is not conforming to UTF-16 encoding");
			}
		}
		return more;
	}

	inline ByteSource *byteSource() const { return byteSource_; }

private:
	Utf16Source(Stream *stream, ByteArray *buf, int endian)
		: byteSource_(ByteSource::open(stream, buf, endian)),
		  firstTime_(true)
	{}

	Ref<ByteSource> byteSource_;
	bool firstTime_;
};

} // namespace fkit

#endif // FKIT_UTF16SOURCE_H
