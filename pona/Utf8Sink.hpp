/*
 * Utf8Sink.hpp -- UTF8 encoding data sink
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_UTF8SINK_HPP
#define PONA_UTF8SINK_HPP

#include "ByteSink.hpp"

namespace pona
{

class Utf8Sink: public ByteSink
{
public:
	Utf8Sink();
	Utf8Sink(Ref<Stream> stream, int bufCapa = PONA_DEFAULT_BUF_CAPA);
	Utf8Sink(void* buf, int bufCapa);
	
	void writeChar(uint32_t ch, bool* valid = 0);
};

} // namespace pona

#endif // PONA_UTF8SINK_HPP
