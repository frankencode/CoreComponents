/*
 * Utf8Sink.cpp -- UTF8 encoding data sink
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */

#include "Utf8Sink.hpp"

namespace pona
{

Utf8Sink::Utf8Sink()
{}

Utf8Sink::Utf8Sink(Ref<Stream> stream, int bufCapa)
	: ByteSink(stream, bufCapa)
{}

Utf8Sink::Utf8Sink(void* buf, int bufCapa)
	: ByteSink(buf, bufCapa)
{}

void Utf8Sink::writeChar(uint32_t ch, bool* valid)
{
	// 0xC = (1100)2
	// 0x8 = (1000)2
	// 0xE = (1110)2
	// 0xF = (1111)2
	
	if (ch < 0x80) {
		writeUInt8(ch);
	}
	else if (ch < 0x800) {
		writeUInt8((ch >> 6) | 0xC0);
		writeUInt8((ch & 0x3F) | 0x80);
	}
	else if (ch < 0x10000) {
		if ((0xD800 <= ch) && (ch <= 0xDFFF)) {
			if (valid) {
				*valid = false;
				return;
			}
			else
				PONA_THROW(StreamEncodingException, "UTF8 disallows encoding of UTF16 surrogate pairs 0xD800..0xDFFF");
		}
		else if ((0xFDD0 <= ch) && (ch <= 0xFDEF)) {
			if (valid) {
				*valid = false;
				return;
			}
			else
				PONA_THROW(StreamEncodingException, "UTF8 disallows encoding of non-characters 0xFDD0..0xFDEF");
		}
		writeUInt8((ch >> 12) | 0xE0);
		writeUInt8(((ch >> 6) & 0x3F) | 0x80);
		writeUInt8((ch & 0x3F) | 0x80);
	}
	else if (ch < 0x11000) {
		if ((ch & 0xFFFE) == 0xFFFE) {
			if (valid) {
				*valid = false;
				return;
			}
			else
				PONA_THROW(StreamEncodingException, "UTF8 disallows encoding of non-characters 0x??FFFE,0x??FFFF");
		}
		writeUInt8((ch >> 18) | 0xF);
		writeUInt8(((ch >> 12) & 0x3F) | 0x80);
		writeUInt8(((ch >> 6) & 0x3F) | 0x80);
		writeUInt8((ch & 0x3F) | 0x80);
	}
	else {
		if (valid) {
			*valid = false;
			return;
		}
		else
			PONA_THROW(StreamEncodingException, "UTF8 disallows encoding of code points above 0x10FFFF");
	}
}

} // namespace pona
