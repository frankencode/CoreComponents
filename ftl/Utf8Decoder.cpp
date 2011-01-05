/*
 * Utf8Decoder.cpp -- UTF8 decoder and validator
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */

#include "Utf8Decoder.hpp"

namespace ftl
{

Utf8Decoder::Utf8Decoder(Ref<Stream> stream, int bufCapa)
	: byteDecoder_(stream, bufCapa)
{}

Utf8Decoder::Utf8Decoder(const void* buf, int bufCapa)
	: byteDecoder_(buf, bufCapa)
{}

uchar_t Utf8Decoder::readMultiByte(uchar_t ch, bool* valid)
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
		uint8_t chs = byteDecoder_.readUInt8();
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
	
	if (valid) {
		*valid = *valid && (n == 0);
	}
	else {
		if (n < 0)
			FTL_THROW(EncodingException, "Input data is not conforming to UTF-8 multibyte encoding");
	}
	
	return ch;
}

} // namespace ftl
