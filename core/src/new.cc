/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <new>
#include "valgrind.h"
#include <cc/Memory>

#ifdef NDEBUG
#define CC_MEMORY_REDZONE_SIZE 0
#else
#define CC_MEMORY_REDZONE_SIZE 16
#endif

void *operator new(std::size_t size) throw (std::bad_alloc) {
    void *data = (void *)((char *)cc::Memory::allocate(size + 2 * CC_MEMORY_REDZONE_SIZE) + CC_MEMORY_REDZONE_SIZE);
    VALGRIND_MALLOCLIKE_BLOCK(data, size, CC_MEMORY_REDZONE_SIZE, /*is_zeroed=*/true);
    return data;
}
void *operator new[](std::size_t size) throw (std::bad_alloc) {
    void *data = (void *)((char *)cc::Memory::allocate(size + 2 * CC_MEMORY_REDZONE_SIZE) + CC_MEMORY_REDZONE_SIZE);
    VALGRIND_MALLOCLIKE_BLOCK(data, size, CC_MEMORY_REDZONE_SIZE, /*is_zeroed=*/true);
    return data;
}
void operator delete(void *data) throw() {
    if (!data) return;
    cc::Memory::free((void *)((char *)data - CC_MEMORY_REDZONE_SIZE));
    VALGRIND_FREELIKE_BLOCK(data, CC_MEMORY_REDZONE_SIZE);
}
void operator delete[](void *data) throw() {
    if (!data) return;
    cc::Memory::free((void *)((char *)data - CC_MEMORY_REDZONE_SIZE));
    VALGRIND_FREELIKE_BLOCK(data, CC_MEMORY_REDZONE_SIZE);
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
