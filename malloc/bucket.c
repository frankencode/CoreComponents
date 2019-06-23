/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include "futex.c"

#pragma pack(push,1)

typedef struct {
    uint32_t futex;
    uint8_t open;
    uint8_t checksum;
    uint16_t prealloc_count;
    uint32_t bytes_dirty;
    uint32_t object_count;
} bucket_t;

#pragma pack(pop)

static_assert(sizeof(bucket_t) == 16, "The bucket header needs to be exactly 16 bytes");

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
