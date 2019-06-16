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
#include <stdlib.h> // exit
#include <unistd.h> // sysconf
#include <errno.h>
#include <stdint.h>
#include <assert.h>

#include "arena.c"

#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS MAP_ANON
#endif

#ifndef MAP_POPULATE
#define MAP_POPULATE 0
#endif

#ifndef MAP_NORESERVE
#define MAP_NORESERVE 0
#endif

inline static size_t round_up_pow2(size_t x, size_t g)
{
    const size_t r = x & (g - 1);
    return r > 0 ? x + g - r : x;
}

void *malloc(size_t size)
{
    arena_t *arena = arena_get();

    bucket_t *bucket = arena->bucket;
    const size_t page_size = arena->page_size;

    assert(__builtin_popcount(CC_MEM_GRANULARITY) == 1);

    size = round_up_pow2(size, CC_MEM_GRANULARITY);

    const size_t bucket_header_size = round_up_pow2(sizeof(bucket_t), CC_MEM_GRANULARITY);

    if (size <= page_size - bucket_header_size)
    {
        int prealloc_count = 0;

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
            else {
                bucket->open = 0;
                int dispose = bucket->object_count == 0;
                prealloc_count = bucket->prealloc_count;
                bucket_release(bucket);
                if (dispose) {
                    bucket_destroy(bucket);
                    cache_push(&arena->cache, bucket, page_size);
                }
            }
        }

        void *page_start = NULL;
        if (prealloc_count > 0) {
            page_start = (uint8_t *)bucket + page_size;
        }
        else {
            page_start = mmap(NULL, page_size * CC_MEM_PAGE_PREALLOC, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE|MAP_NORESERVE|MAP_POPULATE, -1, 0);
            assert(page_start != MAP_FAILED);
        }

        bucket = (bucket_t *)page_start;
        bucket_init(bucket);
        bucket->prealloc_count = (prealloc_count > 0) ? prealloc_count - 1 : CC_MEM_PAGE_PREALLOC - 1;
        bucket->bytes_dirty = bucket_header_size + size;
        bucket->object_count = 1;
        bucket->open = 1;
        arena->bucket = bucket;

        return (uint8_t *)page_start + bucket_header_size;
    }

    if (size <= page_size) {
        void *page_start = mmap(NULL, page_size, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE|MAP_NORESERVE|MAP_POPULATE, -1, 0);
        assert(pageStart != MAP_FAILED);
        return page_start;
    }

    size += sizeof(uint32_t);
    const int page_shift = __builtin_ctz(page_size);
    uint32_t page_count = (size + page_size - 1) >> page_shift;

    void *page_start = mmap(NULL, page_count << page_shift, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE|MAP_NORESERVE/*|MAP_POPULATE*/, -1, 0);
    assert(page_start != MAP_FAILED);
    *(uint32_t *)page_start = page_count;

    return (uint8_t *)page_start + sizeof(uint32_t);
}

void free(void *ptr)
{
    if (ptr == NULL) return;

    arena_t *arena = &thread_local_arena;
    if (!arena) return;

    const size_t page_size = arena->page_size;

    uint32_t offset = (uint32_t)(((uint8_t *)ptr - (uint8_t *)NULL) & (page_size - 1));

    if (offset > sizeof(uint32_t)) {
        bucket_t *bucket = (bucket_t *)((uint8_t *)ptr - offset);
        bucket_acquire(bucket);
        int dispose = ((--bucket->object_count) == 0) && !bucket->open;
        bucket_release(bucket);
        if (dispose) {
            bucket_destroy(bucket);
            cache_push(&arena->cache, bucket, page_size);
        }
    }
    else if (offset == 0) {
        if (munmap(ptr, page_size) == -1) exit(EXIT_FAILURE);
    }
    else if (offset == sizeof(uint32_t)) {
        void *page_start = (uint8_t *)ptr - sizeof(uint32_t);
        uint32_t page_count = *(uint32_t *)page_start;
        if (munmap(page_start, page_count * page_size) == -1) exit(EXIT_FAILURE);
    }
}

void *calloc(size_t number, size_t size)
{
    return malloc(number * size);
}

void *realloc(void *ptr, size_t size)
{
    free(ptr);
    return malloc(size);
}

int posix_memalign(void **ptr, size_t alignment, size_t size)
{
    if (size == 0) {
        *ptr = NULL;
        return 0;
    }

    if ((alignment & (sizeof(void *) - 1)) != 0 || __builtin_popcount(alignment) != 1)
        return EINVAL;

    if (alignment <= CC_MEM_GRANULARITY) {
        *ptr = malloc(size);
        return 0;
    }

    *ptr = malloc(size + alignment);
    size_t r = (size_t)((uint8_t *)ptr - (uint8_t *)NULL) & (alignment - 1);
    if (r > 0) *(uint8_t **)ptr += alignment - r;

    return 0;
}

void *aligned_alloc(size_t alignment, size_t size)
{
    void *ptr = NULL;
    posix_memalign(&ptr, alignment, size);
    return ptr;
}
