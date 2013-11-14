/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "ZStream.h"
#include "InflateStream.h"

namespace flux
{

Ref<InflateStream> InflateStream::open(Stream *source, ByteArray *buf)
{
	return new InflateStream(source, buf);
}

InflateStream::InflateStream(Stream *source, ByteArray *buf)
	: source_(source),
	  zBuf_(buf),
	  z_(ZStream::create())
{
	if (!zBuf_) zBuf_ = ByteArray::create(0x4000);
	int status = inflateInit(z_);
	FLUX_ASSERT(status != Z_STREAM_ERROR);
	FLUX_ASSERT(status != Z_VERSION_ERROR);
	FLUX_ASSERT(status != Z_MEM_ERROR);
	FLUX_ASSERT(status == Z_OK);
}

InflateStream::~InflateStream()
{
	int status = inflateEnd(z_);
	FLUX_ASSERT(status != Z_STREAM_ERROR);
	FLUX_ASSERT(status == Z_OK);
}

bool InflateStream::readyRead(double interval) const
{
	return source_->readyRead(interval);
}

int InflateStream::read(ByteArray *buf)
{
	if (buf->size() == 0) return 0;

	z_->next_in = zBuf_->bytes();
	z_->avail_in = source_->read(zBuf_);
	if (z_->avail_in == 0) return 0;

	z_->next_out = buf->bytes();
	z_->avail_out = buf->size();
	while (true) {
		int status = inflate(z_, Z_NO_FLUSH);
		if (status == Z_OK) {
			if (z_->avail_out == 0) return buf->size();
			if (z_->avail_in == 0) {
				z_->next_in = zBuf_->bytes();
				z_->avail_in = source_->read(zBuf_);
				continue;
			}
		}
		if (status == Z_STREAM_END)
			return buf->size() - z_->avail_out;
		FLUX_ASSERT(status != Z_MEM_ERROR);
		FLUX_ASSERT(status != Z_BUF_ERROR);
		if (status != Z_OK) throw InflateError(status);
	}
}

} // namespace flux
