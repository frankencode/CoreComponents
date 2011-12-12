/*
 * Memory.hpp -- dynamic memory allocation
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_MEMORY_HPP
#define FTL_MEMORY_HPP

#include "ThreadLocalSingleton.hpp"

namespace ftl
{

class Memory: public Instance, public ThreadLocalSingleton<Memory>
{
public:
	static void* allocate(size_t size);
	static void free(void* data);
	
	static size_t pageSize();
	
	void* operator new(size_t size);
	void operator delete(void* data, size_t size);
	
private:
	friend class ThreadLocalSingleton<Memory>;
	
	Memory();
	
	class BucketHeader;
	
	size_t pageSize_;
	BucketHeader* bucket_;
};

inline Ref<Memory> memory() { return Memory::instance(); }

} // namespace ftl

#endif // FTL_MEMORY_HPP
