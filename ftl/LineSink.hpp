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
#include "Array.hpp"
#include "String.hpp"
#include "Stream.hpp"

namespace ftl
{

class LineSink: public Sink<String>
{
public:
	LineSink(Ref<Stream> stream, const char* eol = "\n", int maxLineLength = FTL_DEFAULT_BUF_CAPA);
	
	Ref<Stream> stream() const;
	
	String prefix() const;
	void setPrefix(String prefix);
	
	void writeLine(String line);
	void write(String text);
	
private:
	void feed(const char* data, int size);
	void flush();

	Ref<Stream, Owner> stream_;
	String eol_;
	String prefix_;
	
	int bufFill_;
	Ref<ByteArray, Owner> buf_;
};

} // namespace ftl

#endif // FTL_LINESINK_HPP
