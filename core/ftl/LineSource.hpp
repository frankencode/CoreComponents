 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */
#ifndef FTL_LINESOURCE_HPP
#define FTL_LINESOURCE_HPP

#include "atoms"
#include "defaults.hpp"
#include "string.hpp"
#include "Stream.hpp"

namespace ftl
{

template<class T>
class CircularBuffer;

class LineSource: public Source<string>
{
public:
	inline static hook<LineSource> open(Stream *stream, const char *eol = "\n", int maxLineLength = FTL_DEFAULT_BUF_CAPA) {
		return new LineSource(stream, eol, maxLineLength);
	}

	Stream *stream() const;

	bool read(string *line);
	string readLine();

protected:
	LineSource(Stream *stream, const char *eol, int maxLineLength);

	bool readAvail();
	bool hasMore();

	typedef CircularBuffer<char> Cache;

	hook<Stream> stream_;
	string eol_;
	int cachedLines_;
	hook<Cache> cache_;
	hook<ByteArray> buf_;
};

} // namespace ftl

#endif // FTL_LINESOURCE_HPP
