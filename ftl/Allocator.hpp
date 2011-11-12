/*
 * Allocator.hpp -- dynamic memory allocation
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_ALLOCATOR_HPP
#define FTL_ALLOCATOR_HPP

#include "ThreadLocalSingleton.hpp"

namespace ftl
{

class Allocator: public Instance, public ThreadLocalSingleton<Allocator>
{
public:
	void* allocate(size_t size);
	static void free(void* data);
	
	void* operator new(size_t size);
	void operator delete(void* data, size_t size);
	
private:
	friend class ThreadLocalSingleton<Allocator>;
	
	Allocator();
	
	class BucketHeader;
	
	size_t pageSize_;
	BucketHeader* bucket_;
};

inline Ref<Allocator> allocator() { return Allocator::instance(); }

} // namespace ftl

#endif // FTL_ALLOCATOR_HPP
