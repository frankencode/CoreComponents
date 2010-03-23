/*
 * Utf8Encoder.hpp -- UTF8 encoder
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_UTF8ENCODER_HPP
#define PONA_UTF8ENCODER_HPP

#include "ByteEncoder.hpp"

namespace pona
{

class Utf8Encoder: public Instance
{
public:
	Utf8Encoder();
	Utf8Encoder(Ref<Stream> stream, int bufCapa = PONA_DEFAULT_BUF_CAPA);
	Utf8Encoder(void* buf, int bufCapa);
	
	void writeChar(uint32_t ch, bool* valid = 0);
	
	inline Ref<ByteEncoder> byteEncoder() const { return &byteEncoder_; }
	
private:
	ByteEncoder byteEncoder_;
};

} // namespace pona

#endif // PONA_UTF8ENCODER_HPP
