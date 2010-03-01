/*
 * Utf8Source.hpp -- UTF8 decoder and validator
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_UTF8SOURCE_HPP
#define PONA_UTF8SOURCE_HPP

#include "ByteSource.hpp"

namespace pona
{

class Utf8Source: public ByteSource
{
public:
	Utf8Source(Ref<Stream> stream, int bufCapa = PONA_DEFAULT_BUF_CAPA);
	Utf8Source(const void* buf, int bufCapa);
	
	uint32_t readChar(bool* valid = 0);
};

} // namespace pona

#endif // PONA_UTF8SOURCE_HPP
