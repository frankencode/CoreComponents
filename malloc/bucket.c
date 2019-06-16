/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include "futex.c"

typedef struct {
    uint32_t bytes_dirty;
    uint32_t object_count;
    uint16_t prealloc_count;
    uint16_t open;
    uint32_t futex;
} bucket_t;

inline static void bucket_init(bucket_t *bucket)
{
    bucket->futex = 0;
}

inline static void bucket_destroy(bucket_t *bucket)
{}

inline static void bucket_acquire(bucket_t *bucket)
{
    futex_acquire(&bucket->futex);
}

inline static void bucket_release(bucket_t *bucket)
{
    futex_release(&bucket->futex);
}
