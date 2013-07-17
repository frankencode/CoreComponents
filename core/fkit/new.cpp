/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <new>
#include "valgrind.h"
#include "check.h"
#include "Memory.h"

#ifdef NDEBUG
#define FKIT_MEMORY_REDZONE_SIZE 0
#else
#define FKIT_MEMORY_REDZONE_SIZE 16
#endif

void *operator new(std::size_t size) throw (std::bad_alloc) {
	void *data = (void *)((char *)fkit::Memory::allocate(size + 2 * FKIT_MEMORY_REDZONE_SIZE) + FKIT_MEMORY_REDZONE_SIZE);
	VALGRIND_MALLOCLIKE_BLOCK(data, size, FKIT_MEMORY_REDZONE_SIZE, /*is_zeroed=*/true);
	return data;
}
void *operator new[](std::size_t size) throw (std::bad_alloc) {
	void *data = (void *)((char *)fkit::Memory::allocate(size + 2 * FKIT_MEMORY_REDZONE_SIZE) + FKIT_MEMORY_REDZONE_SIZE);
	VALGRIND_MALLOCLIKE_BLOCK(data, size, FKIT_MEMORY_REDZONE_SIZE, /*is_zeroed=*/true);
	return data;
}
void operator delete(void *data) throw() {
	fkit::Memory::free((void *)((char *)data - FKIT_MEMORY_REDZONE_SIZE));
	VALGRIND_FREELIKE_BLOCK(data, FKIT_MEMORY_REDZONE_SIZE);
}
void operator delete[](void *data) throw() {
	fkit::Memory::free((void *)((char *)data - FKIT_MEMORY_REDZONE_SIZE));
	VALGRIND_FREELIKE_BLOCK(data, FKIT_MEMORY_REDZONE_SIZE);
}

void *operator new(std::size_t size, const std::nothrow_t &) throw() {
	void *data = 0;
	try { data = operator new(size); } catch(...) {}
	return data;
}
void *operator new[](std::size_t size, const std::nothrow_t &) throw() {
	void *data = 0;
	try { data = operator new[](size); } catch(...) {}
	return data;
}
void operator delete(void *data, const std::nothrow_t &) throw() {
	try { operator delete(data); } catch(...) {}
}
void operator delete[](void *data, const std::nothrow_t &) throw() {
	try { operator delete[](data); } catch(...) {}
}
