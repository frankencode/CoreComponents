 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
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
	inline static O<LineSink> open(Stream *stream, const char *eol = "\n", int maxLineLength = FTL_DEFAULT_BUF_CAPA) {
		return new LineSink(stream, eol, maxLineLength);
	}

	Stream *stream() const;

	String prefix() const;
	void setPrefix(String prefix);

	void writeLine(String line);
	void write(String text);

protected:
	LineSink(Stream *stream, const char *eol, int maxLineLength);

	void feed(const char *data, int size);
	void flush();

	O<Stream> stream_;
	String eol_;
	String prefix_;

	int bufFill_;
	O<ByteArray> buf_;
};

} // namespace ftl

#endif // FTL_LINESINK_HPP
