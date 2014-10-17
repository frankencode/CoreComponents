/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/crypto/HashMeter>

namespace flux {
namespace crypto {

Ref<HashMeter> HashMeter::open(HashSum *hashSum, Stream *stream )
{
	return new HashMeter(hashSum, stream);
}

HashMeter::HashMeter(HashSum *hashSum, Stream *stream)
	: hashSum_(hashSum),
	  stream_(stream)
{}

bool HashMeter::readyRead(double interval) const
{
	if (stream_) return stream_->readyRead(interval);
	return false;
}

int HashMeter::read(ByteArray *data)
{
	if (stream_) {
		int n = stream_->read(data);
		hashSum_->feed(ByteRange(data, 0, n));
		return n;
	}
	return 0;
}

void HashMeter::write(const ByteArray *data)
{
	hashSum_->feed(data);
	if (stream_) stream_->write(data);
}

void HashMeter::write(const StringList *parts)
{
	for (int i = 0, n = parts->count(); i < n; ++i)
		hashSum_->feed(parts->at(i));
	if (stream_) stream_->write(parts);
}

Ref<ByteArray> HashMeter::finish()
{
	return hashSum_->finish();
}

}} // namespace flux::crypto
