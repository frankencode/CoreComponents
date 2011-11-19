/*
 * new.cpp -- replacements for the standard memory operators
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */

#include <new>
#include "debug.hpp"
#include "Memory.hpp"

void* operator new(std::size_t size) throw (std::bad_alloc) { return ftl::Memory::allocate(size); }
void* operator new[](std::size_t size) throw (std::bad_alloc) { return ftl::Memory::allocate(size); }
void operator delete(void* data) throw() { return ftl::Memory::free(data); }
void operator delete[](void* data) throw() { return ftl::Memory::free(data); }

void* operator new(std::size_t size, const std::nothrow_t&) throw() { return ftl::Memory::allocate(size); }
void* operator new[](std::size_t size, const std::nothrow_t&) throw() { return ftl::Memory::allocate(size); }
void operator delete(void* data, const std::nothrow_t&) throw() { return ftl::Memory::free(data); }
void operator delete[](void* data, const std::nothrow_t&) throw() { return ftl::Memory::free(data); }
