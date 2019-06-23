/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/malloc>

/// System memory granularity, e.g. XMMS movdqa requires 16
#define CC_MEM_GRANULARITY (2 * sizeof(size_t) < __alignof__ (long double) ? __alignof__ (long double) : 2 * sizeof(size_t))

/// Number of pages to preallocate
#define CC_MEM_PAGE_PREALLOC 16

/// Number of freed pages to cache at maximum
#define CC_MEM_PAGE_CACHE (2 * CC_MEM_PAGE_PREALLOC - 1)

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <threads.h>
#include <stdlib.h> // abort, getenv
#include <unistd.h> // sysconf
#include <string.h> // memcpy
#include <errno.h>
#include <stdint.h>
#include <assert.h>

#ifdef __GNUC__
#define CC_MEM_UNUSED __attribute__ ((unused))
#else
#define CC_MEM_UNUSED inline
#endif

#include "arena.c"
#include "trace.c"

#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS MAP_ANON
#endif

#ifndef MAP_POPULATE
#define MAP_POPULATE 0
#endif

#ifndef MAP_NORESERVE
#define MAP_NORESERVE 0
#endif

#define CC_MEM_IS_POW2(x) (x > 0 && (x & (x - 1)) == 0)

static_assert(CC_MEM_IS_POW2(CC_MEM_GRANULARITY), "CC_MEM_GRANULARITY needs to be a power of two");

inline static size_t round_up_pow2(const size_t x, const size_t g)
{
    const size_t m = g - 1;
    return (x + m) & ~m;
}

void *malloc(size_t size)
{
    arena_t *arena = arena_get();
    if (arena->options & CC_MEM_TRACE) trace_malloc(size);

    const size_t page_size = arena->page_size;

    if (size == 0) return NULL;

    bucket_t *bucket = arena->bucket;

    if (size < arena->half_page_size)
    {
        size = round_up_pow2(size, CC_MEM_GRANULARITY);

        unsigned prealloc_count = 0;

        if (bucket)
        {
            bucket_acquire(bucket);

            if (size <= page_size - bucket->bytes_dirty) {
                void *data = (uint8_t *)bucket + bucket->bytes_dirty;
                bucket->bytes_dirty += size;
                ++bucket->object_count;
                bucket_release(bucket);
                return data;
            }

            bucket->open = 0;
            int dispose = bucket->object_count == 0;
            prealloc_count = bucket->prealloc_count;
            bucket_release(bucket);

            if (dispose) {
                bucket_destroy(bucket);
                cache_push(&arena->cache, bucket, page_size);
            }
        }

        void *page_start = NULL;
        if (prealloc_count > 0) {
            page_start = (uint8_t *)bucket + page_size;
        }
        else {
            page_start = mmap(NULL, arena->prealloc_size, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE|MAP_NORESERVE|MAP_POPULATE, -1, 0);
            if (page_start == MAP_FAILED) {
                errno = ENOMEM;
                return NULL;
            }
            prealloc_count = CC_MEM_PAGE_PREALLOC;
        }
        --prealloc_count;

        const size_t bucket_header_size = round_up_pow2(sizeof(bucket_t), CC_MEM_GRANULARITY);
        bucket = (bucket_t *)page_start;
        bucket_init(bucket);
        bucket->open = 1;
        bucket->prealloc_count = prealloc_count;
        bucket->bytes_dirty = bucket_header_size + size;
        bucket->object_count = 1;
        arena->bucket = bucket;

        return (uint8_t *)page_start + bucket_header_size;
    }

    size = round_up_pow2(size, page_size) + page_size;

    void *head = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE|MAP_NORESERVE|MAP_POPULATE, -1, 0);
    if (head == MAP_FAILED) {
        errno = ENOMEM;
        return NULL;
    }
    *(size_t *)head = size;
    return (uint8_t *)head + page_size;
}

void free(void *ptr)
{
    arena_t *arena = &thread_local_arena;
    if (!arena) return;

    if (arena->options & CC_MEM_TRACE) trace_free(ptr);

    if (ptr == NULL) return;

    const size_t page_size = arena->page_size;
    const size_t page_offset = (size_t)((uint8_t *)ptr - (uint8_t *)NULL) & (page_size - 1);

    if (page_offset > 0) {
        void *page_start = (uint8_t *)ptr - page_offset;
        bucket_t *bucket = (bucket_t *)page_start;
        bucket_acquire(bucket);
        --bucket->object_count;
        int dispose = bucket->object_count == 0 && bucket->open == 0;
        bucket_release(bucket);
        if (dispose) {
            bucket_destroy(bucket);
            cache_push(&arena->cache, bucket, page_size);
        }
    }
    else {
        void *head = (uint8_t *)ptr - page_size;
        if (munmap(head, *(size_t *)head) == -1) abort();
    }
}

void *calloc(size_t number, size_t size)
{
    arena_t *arena = arena_get();
    if (arena->options & CC_MEM_TRACE) trace_calloc(number, size);

    return malloc(number * size);
}

void *realloc(void *ptr, size_t size)
{
    arena_t *arena = arena_get();
    if (arena->options & CC_MEM_TRACE) trace_realloc(ptr, size);

    if (ptr == NULL) return malloc(size);

    if (size == 0) {
        if (ptr != NULL) free(ptr);
        return NULL;
    }

    if (size <= CC_MEM_GRANULARITY) return ptr;

    const size_t page_size = arena->page_size;
    size_t copy_size = page_size;
    size_t page_offset = (size_t)((uint8_t *)ptr - (uint8_t *)NULL) & (page_size - 1);

    if (page_offset > 0) {
        void *page_start = (uint8_t *)ptr - page_offset;
        bucket_t *bucket = (bucket_t *)page_start;
        const size_t size_estimate_1 = bucket->bytes_dirty - page_offset;
        const size_t size_estimate_2 = bucket->bytes_dirty - (bucket->object_count << arena->granularity_shift);
            // size_estimate_2 includes the minimum size of the bucket header
        copy_size = (size_estimate_1 < size_estimate_2) ? size_estimate_1 : size_estimate_2;
    }

    if (copy_size > size) copy_size = size;

    void *new_ptr = malloc(size);
    if (new_ptr == NULL) return NULL;

    memcpy(new_ptr, ptr, copy_size);

    free(ptr);

    return new_ptr;
}

int posix_memalign(void **ptr, size_t alignment, size_t size)
{
    arena_t *arena = arena_get();
    if (arena->options & CC_MEM_TRACE) trace_posix_memalign(ptr, alignment, size);

    if (size == 0) {
        *ptr = NULL;
        return 0;
    }

    if (
        !CC_MEM_IS_POW2(alignment) ||
        (alignment & (sizeof(void *) - 1)) != 0
    )
        return EINVAL;

    if (alignment <= CC_MEM_GRANULARITY) {
        *ptr = malloc(size);
        return (*ptr != NULL) ? 0 : ENOMEM;
    }

    if (alignment + size < arena->half_page_size) {
        uint8_t *ptr_byte = malloc(alignment + size);
        if (ptr_byte != NULL) {
            size_t r = (size_t)(ptr_byte - (uint8_t *)NULL) & (alignment - 1);
            if (r > 0) ptr_byte += alignment - r;
            *ptr = ptr_byte;
            return 0;
        }
    }

    const size_t page_size = arena->page_size;
    size += alignment + page_size;

    void *head = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE|MAP_NORESERVE|MAP_POPULATE, -1, 0);
    if (head == MAP_FAILED) return ENOMEM;

    while (
        (
            (((uint8_t *)head - (uint8_t *)NULL) + page_size)
            & (alignment - 1)
        ) != 0
    ) {
        if (munmap(head, page_size) == -1) abort();
        head = (uint8_t *)head + page_size;
        size -= page_size;
    }

    *(size_t *)head = size;
    *ptr = (uint8_t *)head + page_size;
    return 0;
}

void *aligned_alloc(size_t alignment, size_t size)
{
    arena_t *arena = arena_get();
    if (arena->options & CC_MEM_TRACE) trace_aligned_alloc(alignment, size);

    void *ptr = NULL;
    posix_memalign(&ptr, alignment, size);
    return ptr;
}

void *memalign(size_t alignment, size_t size)
{
    arena_t *arena = arena_get();
    if (arena->options & CC_MEM_TRACE) trace_memalign(alignment, size);

    void *ptr = NULL;
    posix_memalign(&ptr, alignment, size);
    return ptr;
}

void *valloc(size_t size)
{
    arena_t *arena = arena_get();
    if (arena->options & CC_MEM_TRACE) trace_valloc(size);

    return malloc(round_up_pow2(size, arena->page_size));
}

void *pvalloc(size_t size)
{
    arena_t *arena = arena_get();
    if (arena->options & CC_MEM_TRACE) trace_pvalloc(size);

    return malloc(round_up_pow2(size, arena->page_size));
}
