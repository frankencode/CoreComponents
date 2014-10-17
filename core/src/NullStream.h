/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_NULLSTREAM_H
#define FLUX_NULLSTREAM_H

#include <flux/Stream>

namespace flux {

template<class SubClass> class Singleton;

class NullStream: public Stream
{
public:
	virtual int read(ByteArray *buf);
	virtual void write(const ByteArray *buf);

private:
	friend class Singleton<NullStream>;
	NullStream();
};

Stream *nullStream();

} // namespace flux

#endif // FLUX_NULLSTREAM_H
