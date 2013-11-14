/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "ZStream.h"
#include "DeflateStream.h"

namespace flux
{

Ref<DeflateStream> DeflateStream::open(Stream *sink, ByteArray *buf, bool gzip, int level, int strategy)
{
	return new DeflateStream(sink, buf, gzip, level, strategy);
}

DeflateStream::DeflateStream(Stream *sink, ByteArray *buf, bool gzip, int level, int strategy)
	: sink_(sink),
	  zBuf_(buf),
	  z_(ZStream::create())
{
	if (!zBuf_) zBuf_ = ByteArray::create(0x4000);

	if (level < 0) level = Z_DEFAULT_COMPRESSION;
	if (level != Z_DEFAULT_COMPRESSION) {
		if (level < Z_NO_COMPRESSION) level = Z_NO_COMPRESSION;
		else if (level > Z_BEST_COMPRESSION) level = Z_BEST_COMPRESSION;
	}

	if (strategy < 0) strategy = Z_DEFAULT_STRATEGY;
	z_->next_out = zBuf_->bytes();
	z_->avail_out = zBuf_->size();

	int status = deflateInit2(z_, level, Z_DEFLATED, /*windowBits =*/gzip * 16 + 15, /*memLevel =*/8, strategy); // TODO: try: MAX_MEM_LEVEL
	FLUX_ASSERT(status != Z_STREAM_ERROR);
	FLUX_ASSERT(status != Z_VERSION_ERROR);
	FLUX_ASSERT(status != Z_MEM_ERROR);
	FLUX_ASSERT(status == Z_OK);
}

DeflateStream::~DeflateStream()
{
	while (true) {
		int status = deflate(z_, Z_FINISH);
		if (status == Z_OK || status == Z_STREAM_END) {
			if (z_->avail_out == 0) {
				sink_->write(zBuf_);
				z_->next_out = zBuf_->bytes();
				z_->avail_out = zBuf_->size();
			}
			if (status == Z_OK) continue;
			if (z_->avail_out < zBuf_->size()) {
				sink_->write(ByteRange(zBuf_, 0, zBuf_->size() - z_->avail_out));
				status = deflateEnd(z_);
				FLUX_ASSERT(status != Z_STREAM_ERROR);
				FLUX_ASSERT(status != Z_DATA_ERROR);
				FLUX_ASSERT(status == Z_OK);
				break;
			}
		}
		FLUX_ASSERT(status != Z_STREAM_ERROR);
		FLUX_ASSERT(status != Z_BUF_ERROR);
		FLUX_ASSERT(false);
		break;
	}
}

void DeflateStream::write(const ByteArray *buf)
{
	if (buf->size() == 0) return;
	z_->next_in = buf->bytes();
	z_->avail_in = buf->size();
	while (true) {
		int status = deflate(z_, Z_NO_FLUSH);
		FLUX_ASSERT(status != Z_STREAM_ERROR);
		FLUX_ASSERT(status != Z_BUF_ERROR);
		FLUX_ASSERT(status == Z_OK);
		if (z_->avail_out == 0) {
			sink_->write(zBuf_);
			z_->next_out = zBuf_->bytes();
			z_->avail_out = zBuf_->size();
		}
		if (z_->avail_in == 0) break;
	}
}

} // namespace flux
