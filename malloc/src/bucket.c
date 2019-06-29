/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <threads.h>
#include <stdatomic.h>

#pragma pack(push,1)

typedef struct {
    uint32_t lock;
    uint16_t prealloc_count;
    uint16_t page_shift;
    uint32_t bytes_dirty;
    uint32_t object_count;
} bucket_t;

#pragma pack(pop)

static_assert(sizeof(bucket_t) == 16, "The bucket header needs to be exactly 16 bytes");

static thread_local bucket_t *thead_local_bucket = NULL;

inline static void bucket_init(bucket_t *bucket)
{
    bucket->lock = 0;
}

inline static void bucket_destroy(bucket_t *bucket)
{}

inline static void bucket_acquire(bucket_t *bucket)
{
    while (!__sync_bool_compare_and_swap(&bucket->lock, 0, 1)) {
        for (int i = 0; i < 16; ++i) {
            if (__sync_bool_compare_and_swap(&bucket->lock, 0, 1))
                return;
        }
        thrd_yield();
    }
}

inline static void bucket_release(bucket_t *bucket)
{
     bucket->lock = 0;
}
