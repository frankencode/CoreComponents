/*
 * new.cpp -- replacements for the standard memory operators
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */

// #include <stdio.h>
#include <new>
#include "debug.hpp"
#include "Allocator.hpp"

void* operator new(std::size_t size) throw (std::bad_alloc) {
	void* data = ftl::allocator()->allocate(size);
	// printf("new(%d): %p\n", int(size), data);
	return data;
}
void* operator new[](std::size_t size) throw (std::bad_alloc) {
	void* data = ftl::allocator()->allocate(size);
	// printf("new[](%d): %p\n", int(size), data);
	return data;
}
void operator delete(void* data) throw() {
	// printf("delete(%p)\n", data);
	return ftl::Allocator::free(data);
}
void operator delete[](void* data) throw() {
	// printf("delete[](%p)\n", data);
	ftl::check(ftl::allocator() != 0);
	return ftl::Allocator::free(data);
}

void* operator new(std::size_t size, const std::nothrow_t&) throw() { return ftl::allocator()->allocate(size); }
void* operator new[](std::size_t size, const std::nothrow_t&) throw() { return ftl::allocator()->allocate(size); }
void operator delete(void* data, const std::nothrow_t&) throw() { return ftl::Allocator::free(data); }
void operator delete[](void* data, const std::nothrow_t&) throw() { return ftl::Allocator::free(data); }
