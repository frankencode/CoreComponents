/*
 * LineSink.hpp -- canonically buffered data sink
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
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
	ByteArray buf_;
};

} // namespace ftl

#endif // FTL_LINESINK_HPP
