/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "ByteSink.h"
#include "Base64.h"

namespace flux
{

String Base64::encode(const String &source)
{
	const char *alphabet =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz"
		"0123456789+/";

	const int n = source->size();
	int i = 0;

	const int m = 4 * (n / 3 + (n % 3 != 0));
	String sink(m);
	int l = 0;

	while (i < n) {
		uint32_t bits = 0;
		for (int j = 0; j < 3; ++j) {
			bits |= ((i < n) ? uint32_t(source->byteAt(i)) : uint32_t(0));
			bits <<= 8;
			++i;
		}
		bits >>= 8;

		for (int k = 0; k < 4; ++k) {
			sink->at(l++) = alphabet[(bits & 0xfc0000) >> 18];
			bits <<= 6;
		}
	}
	if (i > 0) {
		while (i > n) {
			sink->at(--l) = '=';
			--i;
		}
	}

	return sink;
}

String Base64::decode(const String &source)
{
	FLUX_CHECK(source->size() % 4 == 0, EncodingException, "base64: encoded input needs to be a multiple of 4 bytes");

	const int m = source->size();
	int p = 0;
	while (m - p > 0) {
		++p;
		if (source->at(m - p) != '=') {
			--p;
			break;
		}
	}

	FLUX_CHECK((0 <= p) && (p <= 2), EncodingException, "base64: illegal padding");

	int n = 3 * (m / 4) - p;
	String sink(n);
	int i = 0;

	for (int l = 0; l < m;) {
		uint32_t bits = 0;
		for (int k = 0; k < 4; ++k) {
			uint32_t ch = source->at(l++);
			if (('A' <= ch) && (ch <= 'Z')) ch -= 'A';
			else if (('a' <= ch) && (ch <= 'z')) ch = (ch - 'a') + 26;
			else if (('0' <= ch) && (ch <= '9')) ch = (ch - '0') + 52;
			else if (ch == '+') ch = 62;
			else if (ch == '/') ch = 63;
			else if (ch == '=') ch = 0;
			else FLUX_CHECK(false, EncodingException, "base64: illegal character");
			bits |= ch;
			bits <<= 6;
		}
		bits >>= 6;
		for (int j = 0; j < 3; ++j) {
			if (i == n) break;
			uint8_t ch = (bits & 0xFF0000) >> 16;
			bits <<= 8;
			sink->byteAt(i++) = ch;
		}
		if (i == n) break;
	}

	return sink;
}

}
