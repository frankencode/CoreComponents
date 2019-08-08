/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see kissmalloc/LICENSE).
 *
 */

#include "kissmalloc.h"

////////////////////////////////////////////////////////////////////////////////
/// KISSMALLOC CONFIGURATION
////////////////////////////////////////////////////////////////////////////////

/// System memory granularity, e.g. XMMS movdqa requires 16
#define KISSMALLOC_GRANULARITY (2 * sizeof(size_t) < __alignof__ (long double) ? __alignof__ (long double) : 2 * sizeof(size_t))

/// Number of pages to preallocate
#define KISSMALLOC_PAGE_PREALLOC 64

/// Number of freed pages to cache at maximum (should be N * KISSMALLOC_PAGE_PREALLOC - 1)
#define KISSMALLOC_PAGE_CACHE 255

/// Size of a memory page on the target system
#define KISSMALLOC_PAGE_SIZE 4096

////////////////////////////////////////////////////////////////////////////////
/// INTERNALS...
////////////////////////////////////////////////////////////////////////////////

#define KISSMALLOC_GRANULARITY_SHIFT (__builtin_ctz(KISSMALLOC_GRANULARITY))
#define KISSMALLOC_PAGE_HALF_SIZE (KISSMALLOC_PAGE_SIZE >> 1)
#define KISSMALLOC_PREALLOC_SIZE (KISSMALLOC_PAGE_PREALLOC * KISSMALLOC_PAGE_SIZE)

#include <sys/mman.h>
#include <sys/types.h>
#include <stdlib.h> // abort, getenv
#include <unistd.h> // sysconf
#include <string.h> // memcpy
#include <pthread.h>
#include <errno.h>
#include <stdint.h>
#include <assert.h>

typedef struct {
    int fill;
    void *buffer[KISSMALLOC_PAGE_CACHE];
} cache_t;

static_assert(sizeof(cache_t) <= KISSMALLOC_PAGE_SIZE, "KISSMALLOC_PAGE_CACHE exceeds page size");

inline static void cache_xchg(void **buffer, int i, int j)
{
    void *h = buffer[i];
    buffer[i] = buffer[j];
    buffer[j] = h;
}

inline static int cache_parent(int i)
{
    return (i - 1) >> 1;
}

inline static int cache_child_left(int i)
{
    return (i << 1) + 1;
}

inline static int cache_child_right(int i)
{
    return (i << 1) + 2;
}

inline static int cache_min(void **buffer, int i, int j, int k)
{
    int m = i;

    if (buffer[j] < buffer[k]) {
        if (buffer[j] < buffer[i])
            m = j;
    }
    else if (buffer[k] < buffer[i])
        m = k;

    return m;
}

inline static void cache_bubble_up(cache_t *cache)
{
    void **buffer = cache->buffer;
    for (int i = cache->fill - 1; i > 0;) {
        int j = cache_parent(i);
        if (buffer[i] >= buffer[j]) break;
        cache_xchg(buffer, i, j);
        i = j;
    }
}

inline static void cache_bubble_down(cache_t *cache)
{
    const int fill = cache->fill;
    void **buffer = cache->buffer;
    if (fill == 0) return;
    for (int i = 0; 1;) {
        int lc = cache_child_left(i);
        int rc = cache_child_right(i);
        if (rc < fill) {
            int j = cache_min(buffer, i, lc, rc);
            if (j == i) break;
            cache_xchg(buffer, i, j);
            i = j;
        }
        else if (lc < fill) {
            if (buffer[lc] < buffer[i])
                cache_xchg(buffer, i, lc);
            break;
        }
        else
            break;
    }
}

inline static void *cache_pop(cache_t *cache)
{
    void *page = cache->buffer[0];
    --cache->fill;
    cache->buffer[0] = cache->buffer[cache->fill];
    cache_bubble_down(cache);
    return page;
}

static void cache_reduce(cache_t *cache, int fill_max)
{
    if (cache->fill <= fill_max) return;

    void *chunk = cache_pop(cache);
    size_t size = KISSMALLOC_PAGE_SIZE;
    while (cache->fill > fill_max) {
        void *chunk2 = cache_pop(cache);
        if ((uint8_t *)chunk2 - (uint8_t *)chunk == (ssize_t)size) {
            size += KISSMALLOC_PAGE_SIZE;
        }
        else {
            if (munmap(chunk, size) == -1) abort();
            chunk = chunk2;
            size = KISSMALLOC_PAGE_SIZE;
        }
    }
    if (size > 0) {
        if (munmap(chunk, size) == -1) abort();
    }
}

static cache_t *cache_create()
{
    cache_t *cache = (cache_t *)mmap(NULL, KISSMALLOC_PAGE_SIZE, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE|MAP_NORESERVE|MAP_POPULATE, -1, 0);
    if (cache == MAP_FAILED) abort();
    return cache;
}

static void cache_cleanup(cache_t *cache)
{
    cache_reduce(cache, 0);
    if (munmap(cache, KISSMALLOC_PAGE_SIZE) == -1) abort();
}

static void cache_push(cache_t *cache, void *page)
{
    if (cache->fill == KISSMALLOC_PAGE_CACHE)
        cache_reduce(cache, KISSMALLOC_PAGE_CACHE >> 1);

    cache->buffer[cache->fill] = page;
    ++cache->fill;
    cache_bubble_up(cache);
}

#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS MAP_ANON
#endif

#ifndef MAP_POPULATE
#define MAP_POPULATE 0
#endif

#ifndef MAP_NORESERVE
#define MAP_NORESERVE 0
#endif

#define KISSMALLOC_IS_POW2(x) (x > 0 && (x & (x - 1)) == 0)

static_assert(KISSMALLOC_IS_POW2(KISSMALLOC_GRANULARITY), "KISSMALLOC_GRANULARITY needs to be a power of two");
static_assert(KISSMALLOC_PAGE_SIZE <= 65536, "Page size above 64KiB is not supported");

#pragma pack(push,1)

typedef struct {
    uint32_t prealloc_count; // please keep at the start of the structure to ensure alignment
    uint16_t bytes_dirty;
    uint16_t object_count;
    cache_t *cache;
} bucket_t;

#pragma pack(pop)

static_assert(sizeof(bucket_t) == 16, "The bucket header needs to be exactly 16 bytes");

static pthread_once_t bucket_init_control = PTHREAD_ONCE_INIT;
static pthread_key_t bucket_key;

inline static size_t round_up_pow2(const size_t x, const size_t g)
{
    const size_t m = g - 1;
    return (x + m) & ~m;
}

static void bucket_cleanup(void *arg)
{
    bucket_t *bucket = (bucket_t *)arg;

    if (bucket) {
        cache_cleanup(bucket->cache);

        void *head = bucket;
        size_t size = (bucket->prealloc_count + 1) * KISSMALLOC_PAGE_SIZE;

        if (__sync_sub_and_fetch(&bucket->object_count, 1)) {
            head = ((uint8_t *)head) + KISSMALLOC_PAGE_SIZE;
            size -= KISSMALLOC_PAGE_SIZE;
        }

        if (munmap(head, size) == -1) abort();
    }
}

void bucket_init()
{
    if (pthread_key_create(&bucket_key, bucket_cleanup) != 0) abort();
}

void *KISSMALLOC_NAME(malloc)(size_t size)
{
    pthread_once(&bucket_init_control, bucket_init);
    bucket_t *bucket = (bucket_t *)pthread_getspecific(bucket_key);

    if (size < KISSMALLOC_PAGE_HALF_SIZE)
    {
        size = round_up_pow2(size, KISSMALLOC_GRANULARITY);

        uint32_t prealloc_count = 0;

        if (bucket)
        {
            if (size <= (size_t)KISSMALLOC_PAGE_SIZE - bucket->bytes_dirty) {
                void *data = (uint8_t *)bucket + bucket->bytes_dirty;
                bucket->bytes_dirty += size;
                ++bucket->object_count; // this is atomic on all relevant processors!
                return data;
            }
            prealloc_count = bucket->prealloc_count;

            if (!__sync_sub_and_fetch(&bucket->object_count, 1))
                cache_push(bucket->cache, bucket);
        }

        void *page_start = NULL;
        if (prealloc_count > 0) {
            page_start = (uint8_t *)bucket + KISSMALLOC_PAGE_SIZE;
        }
        else {
            page_start = mmap(NULL, KISSMALLOC_PREALLOC_SIZE, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE|MAP_NORESERVE|MAP_POPULATE, -1, 0);
            if (page_start == MAP_FAILED) {
                errno = ENOMEM;
                return NULL;
            }
            prealloc_count = KISSMALLOC_PAGE_PREALLOC;
        }
        --prealloc_count;

        cache_t *cache = bucket ? bucket->cache : cache_create();

        const size_t bucket_header_size = round_up_pow2(sizeof(bucket_t), KISSMALLOC_GRANULARITY);
        bucket = (bucket_t *)page_start;
        bucket->prealloc_count = prealloc_count;
        bucket->bytes_dirty = bucket_header_size + size;
        bucket->object_count = 2;
        bucket->cache = cache;
        pthread_setspecific(bucket_key, bucket);

        return (uint8_t *)page_start + bucket_header_size;
    }

    size = round_up_pow2(size, KISSMALLOC_PAGE_SIZE) + KISSMALLOC_PAGE_SIZE;

    void *head = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE|MAP_NORESERVE|MAP_POPULATE, -1, 0);
    if (head == MAP_FAILED) {
        errno = ENOMEM;
        return NULL;
    }
    *(size_t *)head = size;
    return (uint8_t *)head + KISSMALLOC_PAGE_SIZE;
}

void KISSMALLOC_NAME(free)(void *ptr)
{
    const size_t page_offset = (size_t)(((uint8_t *)ptr - (uint8_t *)NULL) & (KISSMALLOC_PAGE_SIZE - 1));

    if (page_offset != 0) {
        void *page_start = (uint8_t *)ptr - page_offset;
        bucket_t *bucket = (bucket_t *)page_start;
        if (!__sync_sub_and_fetch(&bucket->object_count, 1)) {
            pthread_once(&bucket_init_control, bucket_init);
            bucket_t *my_bucket = (bucket_t *)pthread_getspecific(bucket_key);
            if (!my_bucket) {
                my_bucket = (bucket_t *)mmap(NULL, KISSMALLOC_PREALLOC_SIZE, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE|MAP_NORESERVE|MAP_POPULATE, -1, 0);
                if (!my_bucket) abort();
                my_bucket->bytes_dirty = round_up_pow2(sizeof(bucket_t), KISSMALLOC_GRANULARITY);
                my_bucket->object_count = 1;
                my_bucket->cache = cache_create();
                pthread_setspecific(bucket_key, my_bucket);
            }
            cache_push(my_bucket->cache, bucket);
        }
    }
    else if (ptr != NULL) {
        void *head = (uint8_t *)ptr - KISSMALLOC_PAGE_SIZE;
        if (munmap(head, *(size_t *)head) == -1) abort();
    }
}

void *KISSMALLOC_NAME(calloc)(size_t number, size_t size)
{
    return malloc(number * size);
}

void *KISSMALLOC_NAME(realloc)(void *ptr, size_t size)
{
    if (ptr == NULL) return malloc(size);

    if (size == 0) {
        if (ptr != NULL) free(ptr);
        return NULL;
    }

    if (size <= KISSMALLOC_GRANULARITY) return ptr;

    size_t copy_size = KISSMALLOC_PAGE_SIZE;
    size_t page_offset = (size_t)((uint8_t *)ptr - (uint8_t *)NULL) & (KISSMALLOC_PAGE_SIZE - 1);

    if (page_offset > 0) {
        void *page_start = (uint8_t *)ptr - page_offset;
        bucket_t *bucket = (bucket_t *)page_start;
        const size_t size_estimate_1 = bucket->bytes_dirty - page_offset;
        const size_t size_estimate_2 = bucket->bytes_dirty - ((bucket->object_count - 1) << KISSMALLOC_GRANULARITY_SHIFT);
            // FIXME: might not work cleanly when reallocating in a different thread
        copy_size = (size_estimate_1 < size_estimate_2) ? size_estimate_1 : size_estimate_2;
    }

    if (copy_size > size) copy_size = size;

    void *new_ptr = malloc(size);
    if (new_ptr == NULL) return NULL;

    memcpy(new_ptr, ptr, copy_size);

    free(ptr);

    return new_ptr;
}

int KISSMALLOC_NAME(posix_memalign)(void **ptr, size_t alignment, size_t size)
{
    if (size == 0) {
        *ptr = NULL;
        return 0;
    }

    if (
        !KISSMALLOC_IS_POW2(alignment) ||
        (alignment & (sizeof(void *) - 1)) != 0
    )
        return EINVAL;

    if (alignment <= KISSMALLOC_GRANULARITY) {
        *ptr = malloc(size);
        return (*ptr != NULL) ? 0 : ENOMEM;
    }

    if (alignment + size < KISSMALLOC_PAGE_HALF_SIZE) {
        uint8_t *ptr_byte = (uint8_t *)malloc(alignment + size);
        if (ptr_byte != NULL) {
            size_t r = (size_t)(ptr_byte - (uint8_t *)NULL) & (alignment - 1);
            if (r > 0) ptr_byte += alignment - r;
            *ptr = ptr_byte;
            return 0;
        }
    }

    size += alignment + KISSMALLOC_PAGE_SIZE;

    void *head = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE|MAP_NORESERVE|MAP_POPULATE, -1, 0);
    if (head == MAP_FAILED) return ENOMEM;

    while (
        (
            (((uint8_t *)head - (uint8_t *)NULL) + KISSMALLOC_PAGE_SIZE)
            & (alignment - 1)
        ) != 0
    ) {
        if (munmap(head, KISSMALLOC_PAGE_SIZE) == -1) abort();
        head = (uint8_t *)head + KISSMALLOC_PAGE_SIZE;
        size -= KISSMALLOC_PAGE_SIZE;
    }

    *(size_t *)head = size;
    *ptr = (uint8_t *)head + KISSMALLOC_PAGE_SIZE;
    return 0;
}

void *KISSMALLOC_NAME(aligned_alloc)(size_t alignment, size_t size)
{
    void *ptr = NULL;
    posix_memalign(&ptr, alignment, size);
    return ptr;
}

void *KISSMALLOC_NAME(memalign)(size_t alignment, size_t size)
{
    void *ptr = NULL;
    posix_memalign(&ptr, alignment, size);
    return ptr;
}

void *KISSMALLOC_NAME(valloc)(size_t size)
{
    return malloc(round_up_pow2(size, KISSMALLOC_PAGE_SIZE));
}

void *KISSMALLOC_NAME(pvalloc)(size_t size)
{
    return malloc(round_up_pow2(size, KISSMALLOC_PAGE_SIZE));
}
