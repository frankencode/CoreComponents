/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <new>
#include "valgrind.h"
#include <flux/check>
#include <flux/Memory>

#ifdef NDEBUG
#define FLUX_MEMORY_REDZONE_SIZE 0
#else
#define FLUX_MEMORY_REDZONE_SIZE 16
#endif

void *operator new(std::size_t size) throw (std::bad_alloc) {
    void *data = (void *)((char *)flux::Memory::allocate(size + 2 * FLUX_MEMORY_REDZONE_SIZE) + FLUX_MEMORY_REDZONE_SIZE);
    VALGRIND_MALLOCLIKE_BLOCK(data, size, FLUX_MEMORY_REDZONE_SIZE, /*is_zeroed=*/true);
    return data;
}
void *operator new[](std::size_t size) throw (std::bad_alloc) {
    void *data = (void *)((char *)flux::Memory::allocate(size + 2 * FLUX_MEMORY_REDZONE_SIZE) + FLUX_MEMORY_REDZONE_SIZE);
    VALGRIND_MALLOCLIKE_BLOCK(data, size, FLUX_MEMORY_REDZONE_SIZE, /*is_zeroed=*/true);
    return data;
}
void operator delete(void *data) throw() {
    if (!data) return;
    flux::Memory::free((void *)((char *)data - FLUX_MEMORY_REDZONE_SIZE));
    VALGRIND_FREELIKE_BLOCK(data, FLUX_MEMORY_REDZONE_SIZE);
}
void operator delete[](void *data) throw() {
    if (!data) return;
    flux::Memory::free((void *)((char *)data - FLUX_MEMORY_REDZONE_SIZE));
    VALGRIND_FREELIKE_BLOCK(data, FLUX_MEMORY_REDZONE_SIZE);
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
