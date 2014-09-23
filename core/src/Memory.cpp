/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <sys/mman.h>
#include <unistd.h>
#include <assert.h>
#include <new>
#include "check.h"
#include "types.h"
#include "Mutex.h"
#include "Memory.h"

#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS MAP_ANON
#endif

namespace flux
{

#define FLUX_MEM_GRANULARITY 16
#define FLUX_MEM_ALIGN(x) ((x) / FLUX_MEM_GRANULARITY + ((x) % FLUX_MEM_GRANULARITY > 0)) * FLUX_MEM_GRANULARITY
	// some CPUs require objects in memory to be aligned to 16 byte boundaries,
	// e.g. the XMMS instruction movdqa may segfault on unaligned arguments

void *Memory::operator new(size_t size)
{
	long pageSize = ::sysconf(_SC_PAGE_SIZE);
	check(pageSize > 0);
	check(size <= (unsigned long)pageSize);
	void *data = ::mmap(0, pageSize, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0);
	check(data != MAP_FAILED);
	return data;
}

void Memory::operator delete(void *data, size_t size)
{
	check(::munmap(data, ::sysconf(_SC_PAGE_SIZE)) == 0);
}

class Memory::BucketHeader: public Mutex
{
public:
	uint32_t bytesDirty_;
	uint32_t objectCount_;
	bool open_;
};

Memory::Memory()
	: pageSize_(::sysconf(_SC_PAGE_SIZE)),
	  bucket_(0)
{}

void *Memory::allocate(size_t size)
{
	Memory *allocator = instance();
	BucketHeader *bucket = allocator->bucket_;
	size_t pageSize = allocator->pageSize_;

	size = FLUX_MEM_ALIGN(size);

	if (size == pageSize) {
		void *pageStart = ::mmap(0, pageSize, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0);
		check(pageStart != MAP_FAILED);
		return pageStart;
	}

	if (!bucket) {
		if (size > pageSize - FLUX_MEM_ALIGN(sizeof(BucketHeader))) {
			size += sizeof(uint32_t);
			uint32_t pageCount = size / pageSize + ((size % pageSize) > 0);
			void *pageStart = ::mmap(0, pageCount * pageSize, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0);
			check(pageStart != MAP_FAILED);
			*(uint32_t *)pageStart = pageCount;
			return (void *)((uint32_t *)pageStart + 1);
		}
		else {
			void *pageStart = ::mmap(0, pageSize, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0);
			check(pageStart != MAP_FAILED);
			bucket = new(pageStart)BucketHeader;
			check((char*)bucket == (char*)pageStart);
			bucket->bytesDirty_ = FLUX_MEM_ALIGN(sizeof(BucketHeader)) + size;
			bucket->objectCount_ = 1;
			bucket->open_ = true;
			allocator->bucket_ = bucket;
			return (void *)((char *)pageStart + FLUX_MEM_ALIGN(sizeof(BucketHeader)));
		}
	}
	else {
		bucket->acquire();
		if (size <= pageSize - bucket->bytesDirty_) {
			void *data = (void *)(((char *)bucket) + bucket->bytesDirty_);
			bucket->bytesDirty_ += size;
			++bucket->objectCount_;
			bucket->release();
			return data;
		}
		else {
			bucket->open_ = false;
			bool dispose = (bucket->objectCount_ == 0);
			bucket->release();
			if (dispose) {
				bucket->~BucketHeader();
				check(::munmap((void *)bucket, pageSize) == 0);
			}
			allocator->bucket_ = 0;
			return allocate(size);
		}
	}
}

void Memory::free(void *data)
{
	Memory *allocator = instance();
	size_t pageSize = allocator ? allocator->pageSize_ : size_t(::sysconf(_SC_PAGE_SIZE));

	uint32_t offset = ((char *)data - (char *)0) % pageSize;

	if (offset == 0) {
		check(::munmap(data, pageSize) == 0);
	}
	else if (offset == sizeof(uint32_t)) {
		void *pageStart = (void *)((char *)data - sizeof(uint32_t));
		uint32_t pageCount = *(uint32_t *)pageStart;
		check(::munmap(pageStart, pageCount * pageSize) == 0);
	}
	else {
		BucketHeader *bucket = (BucketHeader *)((char *)data - offset);
		bucket->acquire();
		bool dispose = ((--bucket->objectCount_) == 0) && !bucket->open_;
		bucket->release();
		if (dispose) {
			bucket->~BucketHeader();
			check(::munmap((void *)((char *)data - offset), pageSize) == 0);
		}
	}
}

size_t Memory::pageSize()
{
	return instance()->pageSize_;
}

} // namespace flux
