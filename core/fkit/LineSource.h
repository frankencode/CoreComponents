/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FKIT_LINESOURCE_H
#define FKIT_LINESOURCE_H

#include "String.h"
#include "Stream.h"

namespace fkit
{

template<class T>
class CircularBuffer;

class LineSource: public Source<String>
{
public:
	inline static Ref<LineSource> open(Stream *stream, const char *eol = "\n", int maxLineLength = 0x4000) {
		return new LineSource(stream, eol, maxLineLength);
	}

	Stream *stream() const;

	bool read(String *line);
	String readLine();

protected:
	LineSource(Stream *stream, const char *eol, int maxLineLength);

	bool readAvail();
	bool hasMore();

	typedef CircularBuffer<char> Cache;

	Ref<Stream> stream_;
	String eol_;
	int cachedLines_;
	Ref<Cache> cache_;
	Ref<ByteArray> buf_;
};

} // namespace fkit

#endif // FKIT_LINESOURCE_H
