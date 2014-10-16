/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXSTREAM_LOOKAHEADSTREAM_H
#define FLUXSTREAM_LOOKAHEADSTREAM_H

#include <flux/Stream>

namespace flux {
namespace stream {

class LookAheadStream: public Stream
{
public:
	static Ref<LookAheadStream> open(Stream *source, int windowSize);

	virtual bool readyRead(double interval) const;
	virtual int read(ByteArray *data);
	virtual off_t transfer(off_t count = -1, Stream *sink = 0, ByteArray *buf = 0);

	void replay();
	void done();

private:
	LookAheadStream(Stream *source, int windowSize);

	Ref<Stream> source_;
	Ref<ByteArray> window_;
	const int w_; // window size
	int m_; // window fill
	int i_; // read offset
	bool done_;
};

}} // namespace flux::stream

#endif // FLUXSTREAM_LOOKAHEADSTREAM_H
