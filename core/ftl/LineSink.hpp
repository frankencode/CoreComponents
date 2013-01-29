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
#include "string.hpp"
#include "Stream.hpp"

namespace ftl
{

class LineSink: public Sink<string>
{
public:
	inline static hook<LineSink> open(Stream *stream, const char *eol = "\n", int maxLineLength = FTL_DEFAULT_BUF_CAPA) {
		return new LineSink(stream, eol, maxLineLength);
	}

	Stream *stream() const;

	string prefix() const;
	void setPrefix(string prefix);

	void writeLine(string line);
	void write(string text);

protected:
	LineSink(Stream *stream, const char *eol, int maxLineLength);

	void feed(const char *data, int size);
	void flush();

	hook<Stream> stream_;
	string eol_;
	string prefix_;

	int bufFill_;
	hook<ByteArray> buf_;
};

} // namespace ftl

#endif // FTL_LINESINK_HPP
