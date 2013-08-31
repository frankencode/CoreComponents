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

class LineSource: public Source<String>
{
public:
	static Ref<LineSource> open(ByteArray *buf);
	static Ref<LineSource> open(Stream *stream, ByteArray *buf = 0);

	inline Stream *stream() const { return stream_; }
	inline ByteArray *buf() const { return buf_; }

	bool read(String *line);
	String readLine();

	String pendingData() const;

private:
	LineSource(Stream *stream, ByteArray *buf);

	int findEol(ByteArray *buf, int n, int i) const;
	int skipEol(ByteArray *buf, int n, int i) const;

	Ref<Stream> stream_;
	String buf_;
	bool eoi_;
	int i_, n_;
};

} // namespace fkit

#endif // FKIT_LINESOURCE_H
