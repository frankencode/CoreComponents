#include "kissmalloc.h"

#include <new>

#ifndef KISSMALLOC_VALGRIND
#ifndef NDEBUG
#define KISSMALLOC_VALGRIND
#endif
#endif

#ifdef KISSMALLOC_VALGRIND
#include <valgrind/valgrind.h>
#ifndef KISSMALLOC_REDZONE_SIZE
#ifdef NDEBUG
#define KISSMALLOC_REDZONE_SIZE 0
#else
#define KISSMALLOC_REDZONE_SIZE 16
#endif
#endif
#endif

void *operator new(std::size_t size)
{
    #ifndef KISSMALLOC_VALGRIND
    void *data = KISSMALLOC_NAME(malloc)(size);
    #else
    #ifndef KISSMALLOC_NAME_PREFIX
    void *data = malloc(size);
    #else
    void *data = (void *)((char *)KISSMALLOC_NAME(malloc)(size + 2 * KISSMALLOC_REDZONE_SIZE) + KISSMALLOC_REDZONE_SIZE);
    VALGRIND_MALLOCLIKE_BLOCK(data, size, KISSMALLOC_REDZONE_SIZE, /*is_zeroed=*/true);
    #endif
    #endif
    if (!data) throw std::bad_alloc{};
    return data;
}

void *operator new[](std::size_t size)
{
    #ifndef KISSMALLOC_VALGRIND
    void *data = KISSMALLOC_NAME(malloc)(size);
    #else
    #ifndef KISSMALLOC_NAME_PREFIX
    void *data = malloc(size);
    #else
    void *data = (void *)((char *)KISSMALLOC_NAME(malloc)(size + 2 * KISSMALLOC_REDZONE_SIZE) + KISSMALLOC_REDZONE_SIZE);
    VALGRIND_MALLOCLIKE_BLOCK(data, size, KISSMALLOC_REDZONE_SIZE, /*is_zeroed=*/true);
    #endif
    #endif
    if (!data) throw std::bad_alloc{};
    return data;
}

void operator delete(void *data) noexcept
{
    #ifndef KISSMALLOC_VALGRIND
    KISSMALLOC_NAME(free)(data);
    #else
    #ifndef KISSMALLOC_NAME_PREFIX
    free(data);
    #else
    KISSMALLOC_NAME(free)((void *)((char *)data - KISSMALLOC_REDZONE_SIZE));
    VALGRIND_FREELIKE_BLOCK(data, KISSMALLOC_REDZONE_SIZE);
    #endif
    #endif
}

void operator delete[](void *data) noexcept
{
    #ifndef KISSMALLOC_VALGRIND
    KISSMALLOC_NAME(free)(data);
    #else
    #ifndef KISSMALLOC_NAME_PREFIX
    free(data);
    #else
    KISSMALLOC_NAME(free)((void *)((char *)data - KISSMALLOC_REDZONE_SIZE));
    VALGRIND_FREELIKE_BLOCK(data, KISSMALLOC_REDZONE_SIZE);
    #endif
    #endif
}

void *operator new(std::size_t size, const std::nothrow_t &) noexcept
{
    #ifndef KISSMALLOC_VALGRIND
    void *data = KISSMALLOC_NAME(malloc)(size);
    #else
    #ifndef KISSMALLOC_NAME_PREFIX
    void *data = malloc(size);
    #else
    void *data = (void *)((char *)KISSMALLOC_NAME(malloc)(size + 2 * KISSMALLOC_REDZONE_SIZE) + KISSMALLOC_REDZONE_SIZE);
    VALGRIND_MALLOCLIKE_BLOCK(data, size, KISSMALLOC_REDZONE_SIZE, /*is_zeroed=*/true);
    #endif
    #endif
    return data;
}

void *operator new[](std::size_t size, const std::nothrow_t &) noexcept
{
    #ifndef KISSMALLOC_VALGRIND
    void *data = KISSMALLOC_NAME(malloc)(size);
    #else
    #ifndef KISSMALLOC_NAME_PREFIX
    void *data = malloc(size);
    #else
    void *data = (void *)((char *)KISSMALLOC_NAME(malloc)(size + 2 * KISSMALLOC_REDZONE_SIZE) + KISSMALLOC_REDZONE_SIZE);
    VALGRIND_MALLOCLIKE_BLOCK(data, size, KISSMALLOC_REDZONE_SIZE, /*is_zeroed=*/true);
    #endif
    #endif
    return data;
}

void operator delete(void *data, const std::nothrow_t &) noexcept
{
    #ifndef KISSMALLOC_VALGRIND
    KISSMALLOC_NAME(free)(data);
    #else
    #ifndef KISSMALLOC_NAME_PREFIX
    free(data);
    #else
    KISSMALLOC_NAME(free)((void *)((char *)data - KISSMALLOC_REDZONE_SIZE));
    VALGRIND_FREELIKE_BLOCK(data, KISSMALLOC_REDZONE_SIZE);
    #endif
    #endif
}

void operator delete[](void *data, const std::nothrow_t &) noexcept
{
    #ifndef KISSMALLOC_VALGRIND
    KISSMALLOC_NAME(free)(data);
    #else
    #ifndef KISSMALLOC_NAME_PREFIX
    free(data);
    #else
    KISSMALLOC_NAME(free)((void *)((char *)data - KISSMALLOC_REDZONE_SIZE));
    VALGRIND_FREELIKE_BLOCK(data, KISSMALLOC_REDZONE_SIZE);
    #endif
    #endif
}
