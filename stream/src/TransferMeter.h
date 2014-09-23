/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXSTREAM_TRANSFERMETER_H
#define FLUXSTREAM_TRANSFERMETER_H

#include <flux/Stream.h>

namespace flux {
namespace stream {

class TransferMeter: public Stream
{
public:
	static Ref<TransferMeter> open(Stream *stream);

	inline Stream *stream() const { return stream_; }
	inline size_t totalRead() const { return totalRead_; }
	inline size_t totalWritten() const { return totalWritten_; }

	virtual bool readyRead(double interval) const;
	virtual int read(ByteArray *buf);
	virtual void write(const ByteArray *buf);
	virtual void write(const StringList *parts);

private:
	TransferMeter(Stream *stream);

	Ref<Stream> stream_;
	size_t totalRead_;
	size_t totalWritten_;
};

}} // namespace flux::stream

#endif // FLUXSTREAM_TRANSFERMETER_H
