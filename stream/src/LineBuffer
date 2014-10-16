/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXSTREAM_LINEBUFFER_H
#define FLUXSTREAM_LINEBUFFER_H

#include <flux/Stream>

namespace flux {
namespace stream {

class LineBuffer: public Stream
{
public:
	static Ref<LineBuffer> open(Stream *stream, String prefix = "");

	inline Stream *stream() const { return stream_; }
	virtual String prefix() const;

	virtual bool readyRead(double interval) const;
	virtual int read(ByteArray *buf);

	virtual void write(const ByteArray *buf);
	virtual void write(const StringList *parts);

protected:
	LineBuffer(Stream *stream, String prefix = "");

private:
	Ref<Stream> stream_;
	String prefix_;
	Ref<StringList> backlog_;
};

}} // namespace flux::stream

#endif // FLUXSTREAM_LINEBUFFER_H
