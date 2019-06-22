/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include "bucket.c"
#include "cache.c"

enum {
    CC_MEM_TRACE = 1,
};

typedef struct {
    bucket_t *bucket;
    cache_t cache;
    size_t page_size;
    int options;
} arena_t;

static thread_local arena_t thread_local_arena = {
    page_size: 0,
    bucket: NULL,
    cache: { fill: 0 },
    options: 0
};

static void arena_cleanup(void *value)
{
    arena_t *arena = (arena_t *)value;
    cache_reduce(&arena->cache, 0, arena->page_size);
}

inline static arena_t *arena_get()
{
    arena_t *arena = &thread_local_arena;

    if (arena->page_size == 0) {
        arena->page_size = sysconf(_SC_PAGE_SIZE);
        if (getenv("CC_MEM_TRACE")) arena->options |= CC_MEM_TRACE;
        tss_t key;
        if (tss_create(&key, arena_cleanup) == thrd_error) abort();
        if (tss_set(key, arena) == thrd_error) abort();
    }

    return arena;
}
