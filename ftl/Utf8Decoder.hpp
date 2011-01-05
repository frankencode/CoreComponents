/*
 * Utf8Decoder.hpp -- UTF8 decoder and validator
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_UTF8DECODER_HPP
#define FTL_UTF8DECODER_HPP

#include "ByteDecoder.hpp"

namespace ftl
{

class Utf8Decoder: public Source<uchar_t>
{
public:
	Utf8Decoder(Ref<Stream> stream, int bufCapa = FTL_DEFAULT_BUF_CAPA);
	Utf8Decoder(const void* buf, int bufCapa);
	
	inline bool hasNext() { return byteDecoder_.hasNext(); }
	inline uchar_t next() { return readChar(); }
	
	inline uchar_t readChar(bool* valid = 0) {
		uchar_t ch = byteDecoder_.readUInt8();
		if ((ch & 0x80) != 0) // distinguish 7 bit ASCII from multibyte sequence
			ch = readMultiByte(ch, valid);
		return ch;
	}
	
	inline Ref<ByteDecoder> byteDecoder() const { return &byteDecoder_; }
	
private:
	uchar_t readMultiByte(uchar_t ch, bool* valid);
	
	ByteDecoder byteDecoder_;
};

} // namespace ftl

#endif // FTL_UTF8DECODER_HPP
