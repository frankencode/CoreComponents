/*
 * Utf8Decoder.hpp -- UTF8 decoder and validator
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_UTF8DECODER_HPP
#define PONA_UTF8DECODER_HPP

#include "ByteDecoder.hpp"

namespace pona
{

class Utf8Decoder: public Source<uchar_t>
{
public:
	Utf8Decoder(Ref<Stream> stream, int bufCapa = PONA_DEFAULT_BUF_CAPA);
	Utf8Decoder(const void* buf, int bufCapa);
	
	inline bool hasNext() { return byteDecoder_.hasNext(); }
	inline uchar_t next() { return readChar(); }
	
	uchar_t readChar(bool* valid = 0);
	
	inline Ref<ByteDecoder> byteDecoder() const { return &byteDecoder_; }
	
private:
	ByteDecoder byteDecoder_;
};

} // namespace pona

#endif // PONA_UTF8DECODER_HPP
