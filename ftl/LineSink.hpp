/*
 * LineSink.hpp -- canonically buffered data sink
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_LINESINK_HPP
#define FTL_LINESINK_HPP

#include "atoms"
#include "defaults.hpp"
#include "String.hpp"
#include "Stream.hpp"

namespace ftl
{

class LineSink: public Sink<String>
{
public:
	LineSink(Ref<Stream> stream, int bufCapa = FTL_DEFAULT_BUF_CAPA, const char* eol = "\n");
	~LineSink();
	
	void writeLine(String line);
	void write(String s);
	
	Ref<Stream> stream() const;
	
private:
	Ref<Stream, Owner> stream_;
	String eol_;
	int bufCapa_;
	uint8_t* buf_;
};

} // namespace ftl

#endif // FTL_LINESINK_HPP
