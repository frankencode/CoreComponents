/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_MEMORY_H
#define FLUX_MEMORY_H

#include <flux/ThreadLocalSingleton>

namespace flux {

class Memory: public Object, public ThreadLocalSingleton<Memory>
{
public:
	static void *allocate(size_t size);
	static void free(void *data);

	static size_t pageSize();

	void *operator new(size_t size);
	void operator delete(void *data, size_t size);

private:
	friend class ThreadLocalSingleton<Memory>;

	Memory();

	class BucketHeader;

	size_t pageSize_;
	BucketHeader *bucket_;
};

inline Memory *memory() { return Memory::instance(); }

} // namespace flux

#endif // FLUX_MEMORY_H
