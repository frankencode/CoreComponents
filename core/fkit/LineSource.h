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
	inline static Ref<LineSource> open(Stream *stream, ByteArray *buf = 0) {
		return new LineSource(stream, buf);
	}

	inline Stream *stream() const { return stream_; }

	bool read(String *line);
	String readLine();

	String pendingData() const;

protected:
	LineSource(Stream *stream, ByteArray *buf);

	virtual int findEol(ByteArray *buf, int n, int i) const;
	virtual int skipEol(ByteArray *buf, int n, int i) const;

private:
	Ref<Stream> stream_;
	bool eoi_;
	String buf_;
	int i_, n_;
};

} // namespace fkit

#endif // FKIT_LINESOURCE_H
