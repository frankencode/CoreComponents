/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_UTF8_H
#define FLUX_UTF8_H

#include <flux/exceptions>

namespace flux {

namespace utf8
{

inline int encodedSize(uchar_t ch)
{
	int n = 0;
	if (ch < 0x80)         n = 1; // ASCII range: 0xxxxxxx
	else if (ch < 0x800)   n = 2; // two-byte codes: 110yyyxx | 10xxxxx
	else if (ch < 0x10000) n = 3; // three-byte codes: 1110yyyy | 10yyyyxx | 10xxxxxx
	else if (ch < 0x11000) n = 4; // four-byte codes: 11110zzz | 10zzyyyy | 10yyyyxx | 10xxxxxx
	return n;
}

class EncodeSurrogatePairError: public EncodingError
{
public:
	~EncodeSurrogatePairError() throw() {}

	virtual String message() const {
		return "UTF-8 error: encoding surrogate pairs is not allowed (0xD800..0xDFFF)";
	}
};

class EncodeNonCharacterError: public EncodingError
{
public:
	~EncodeNonCharacterError() throw() {}

	virtual String message() const {
		return "UTF-8 error: encoding non-characters is not allowed (0xFDD0..0xFFFE, 0x*FFF(E|F))";
	}
};

class EncodeLargeCodePointError: public EncodingError
{
public:
	~EncodeLargeCodePointError() throw() {}

	virtual String message() const {
		return "UTF-8 error: code point to large (>= 0x110000)";
	}
};

class DecodeError: public EncodingError
{
public:
	~DecodeError() throw() {}

	virtual String message() const {
		return "UTF-8 error: failed to decode input bytes";
	}
};

} // namespace utf8

} // namespace flux

#endif // FLUX_UTF8_H
