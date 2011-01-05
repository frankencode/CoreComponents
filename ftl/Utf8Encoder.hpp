/*
 * Utf8Encoder.hpp -- UTF8 encoder
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_UTF8ENCODER_HPP
#define FTL_UTF8ENCODER_HPP

#include "ByteEncoder.hpp"

namespace ftl
{

class Utf8Encoder: public Instance
{
public:
	Utf8Encoder();
	Utf8Encoder(Ref<Stream> stream, int bufCapa = FTL_DEFAULT_BUF_CAPA);
	Utf8Encoder(void* buf, int bufCapa);
	
	void writeChar(uchar_t ch, bool* valid = 0);
	
	inline Ref<ByteEncoder> byteEncoder() const { return &byteEncoder_; }
	
private:
	ByteEncoder byteEncoder_;
};

} // namespace ftl

#endif // FTL_UTF8ENCODER_HPP
