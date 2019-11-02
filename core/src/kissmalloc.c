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

/// Number of pages to preallocate
#ifndef KISSMALLOC_PAGE_PREALLOC
#define KISSMALLOC_PAGE_PREALLOC 256
#endif

/// Number of freed pages to cache at maximum (should be N * KISSMALLOC_PAGE_PREALLOC - 1)
#ifndef KISSMALLOC_PAGE_CACHE
#define KISSMALLOC_PAGE_CACHE 511
#endif

/// System memory granularity, e.g. XMMS movdqa requires 16
#ifndef KISSMALLOC_GRANULARITY
#define KISSMALLOC_GRANULARITY 16 // TODO: autodetect
#endif

/// Page size (0 for autodetect)
#ifndef KISSMALLOC_PAGE_SIZE
#define KISSMALLOC_PAGE_SIZE 0
#endif

////////////////////////////////////////////////////////////////////////////////
/// INTERNALS...
////////////////////////////////////////////////////////////////////////////////

#define KISSMALLOC_GRANULARITY_SHIFT (__builtin_ctz(KISSMALLOC_GRANULARITY))

#include <sys/mman.h>
#include <sys/types.h>
#include <stdlib.h> // abort, getenv
#include <unistd.h> // sysconf
#include <string.h> // memcpy
#include <pthread.h>
#include <errno.h>
#include <stdint.h>
#include <assert.h>

#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS MAP_ANON
#endif

#ifndef MAP_POPULATE
#define MAP_POPULATE 0
#endif

#define KISSMALLOC_IS_POW2(x) (x > 0 && (x & (x - 1)) == 0)

static_assert(KISSMALLOC_IS_POW2(KISSMALLOC_GRANULARITY), "KISSMALLOC_GRANULARITY needs to be a power of two");

#pragma pack(push,1)

struct cache_t;

struct bucket_t {
    uint32_t object_count; // please keep at the beginning of the structure for alignment/atomicity reason
    uint32_t bytes_free;
    struct cache_t *cache;
};

struct cache_t {
    uint32_t prealloc_count;
    uint32_t fill;
    struct bucket_t *buffer[KISSMALLOC_PAGE_CACHE];
};

#pragma pack(pop)

static_assert(sizeof(struct bucket_t) <= KISSMALLOC_GRANULARITY, "The bucket_t header must not exceed KISSMALLOC_GRANULARITY bytes");

inline static size_t round_up_pow2(const size_t x, const size_t g)
{
    const size_t m = g - 1;
    return (x + m) & ~m;
}

inline static size_t page_size_get()
{
    #if KISSMALLOC_PAGE_SIZE > 0
    return KISSMALLOC_PAGE_SIZE;
    #else
    static size_t page_size = 0;
    if (page_size == 0) page_size = sysconf(_SC_PAGESIZE);
    return page_size;
    #endif
}

inline static void cache_xchg(struct bucket_t **buffer, int i, int j)
{
    struct bucket_t *h = buffer[i];
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

inline static int cache_min(struct bucket_t **buffer, int i, int j, int k)
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

inline static void cache_bubble_up(struct cache_t *cache)
{
    struct bucket_t **buffer = cache->buffer;
    for (int i = cache->fill - 1; i > 0;) {
        int j = cache_parent(i);
        if (buffer[i] >= buffer[j]) break;
        cache_xchg(buffer, i, j);
        i = j;
    }
}

inline static void cache_bubble_down(struct cache_t *cache)
{
    const int fill = cache->fill;
    struct bucket_t **buffer = cache->buffer;
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

inline static struct bucket_t *cache_pop(struct cache_t *cache)
{
    struct bucket_t *page = cache->buffer[0];
    --cache->fill;
    cache->buffer[0] = cache->buffer[cache->fill];
    cache_bubble_down(cache);
    return page;
}

static void cache_reduce(struct cache_t *cache, uint32_t fill_max)
{
    if (cache->fill <= fill_max) return;

    const size_t page_size = page_size_get();

    void *chunk = cache_pop(cache);
    size_t size = page_size_get();

    while (cache->fill > fill_max) {
        void *chunk2 = cache_pop(cache);
        if ((uint8_t *)chunk2 - (uint8_t *)chunk == (ssize_t)size) {
            size += page_size;
        }
        else {
            if (munmap(chunk, size) == -1) abort();
            chunk = chunk2;
            size = page_size;
        }
    }

    if (size > 0) {
        if (munmap(chunk, size) == -1) abort();
    }
}

inline static size_t cache_size_get()
{
    return round_up_pow2(sizeof(struct cache_t), page_size_get());
}

static struct cache_t *cache_create()
{
    struct cache_t *cache = (struct cache_t *)mmap(NULL, cache_size_get(), PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE|MAP_POPULATE, -1, 0);
    if (cache == MAP_FAILED) abort();
    return cache;
}

static void cache_cleanup(struct cache_t *cache)
{
    cache_reduce(cache, 0);
    if (munmap(cache, cache_size_get()) == -1) abort();
}

static void cache_push(struct cache_t *cache, struct bucket_t *page)
{
    if (cache->fill == KISSMALLOC_PAGE_CACHE)
        cache_reduce(cache, KISSMALLOC_PAGE_CACHE >> 1);

    cache->buffer[cache->fill] = page;
    ++cache->fill;
    cache_bubble_up(cache);
}

static pthread_once_t bucket_key_init_control = PTHREAD_ONCE_INIT;
static pthread_key_t bucket_key = 0;

static void bucket_cleanup(void *arg)
{
    struct bucket_t *bucket = (struct bucket_t *)arg;

    if (bucket) {
        const size_t page_size = page_size_get();

        void *head = bucket;
        size_t size = (bucket->cache->prealloc_count + 1) * page_size;

        cache_cleanup(bucket->cache);

        if (__sync_sub_and_fetch(&bucket->object_count, 1)) {
            head = ((uint8_t *)head) + page_size;
            size -= page_size;
        }

        if (munmap(head, size) == -1) abort();
    }
}

static void bucket_key_init()
{
    if (pthread_key_create(&bucket_key, bucket_cleanup) != 0) abort();
}

static struct bucket_t *bucket_create_initial()
{
    const size_t page_size = page_size_get();

    void *page_start = mmap(NULL, KISSMALLOC_PAGE_PREALLOC * page_size, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0);
    if (page_start == MAP_FAILED) abort();

    struct cache_t *cache = cache_create();
    cache->prealloc_count = KISSMALLOC_PAGE_PREALLOC - 1;

    const size_t bucket_header_size = round_up_pow2(sizeof(struct bucket_t), KISSMALLOC_GRANULARITY);
    struct bucket_t *bucket = (struct bucket_t *)page_start;
    bucket->bytes_free = page_size - bucket_header_size;
    bucket->object_count = 1;
    bucket->cache = cache;

    pthread_once(&bucket_key_init_control, bucket_key_init);
    pthread_setspecific(bucket_key, bucket);

    return bucket;
}

static void *bucket_advance(struct bucket_t *bucket, const size_t page_size, const size_t item_size)
{
    uint32_t prealloc_count = bucket->cache->prealloc_count;
    struct cache_t *cache = bucket->cache;

    if (!__sync_sub_and_fetch(&bucket->object_count, 1))
        cache_push(bucket->cache, bucket);

    void *page_start = NULL;
    if (prealloc_count > 0) {
        page_start = (uint8_t *)bucket + page_size;
        --prealloc_count;
    }
    else {
        page_start = mmap(NULL, KISSMALLOC_PAGE_PREALLOC * page_size, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0);
        if (page_start == MAP_FAILED) {
            errno = ENOMEM;
            return NULL;
        }
        prealloc_count = KISSMALLOC_PAGE_PREALLOC - 1;
    }

    cache->prealloc_count = prealloc_count;

    const size_t bucket_header_size = round_up_pow2(sizeof(struct bucket_t), KISSMALLOC_GRANULARITY);

    bucket = (struct bucket_t *)page_start;
    bucket->bytes_free = page_size - bucket_header_size - item_size;
    bucket->object_count = 2;
    bucket->cache = cache;

    pthread_setspecific(bucket_key, bucket);

    return (uint8_t *)page_start + bucket_header_size;
}

inline static struct bucket_t *bucket_get_mine()
{
    struct bucket_t *bucket = (struct bucket_t *)pthread_getspecific(bucket_key);
    if (bucket == NULL) bucket = bucket_create_initial();
    return bucket;
}

void *KISSMALLOC_NAME(malloc)(size_t size)
{
    const size_t page_size = page_size_get();

    if (size < page_size >> 1)
    {
        if (size == 0) return NULL;

        size = round_up_pow2(size, KISSMALLOC_GRANULARITY);

        struct bucket_t *bucket = bucket_get_mine();

        if (size <= bucket->bytes_free) {
            void *data = (uint8_t *)bucket + page_size - bucket->bytes_free;
            bucket->bytes_free -= size;
            ++bucket->object_count;
            return data;
        }

        return bucket_advance(bucket, page_size, size);
    }

    size = round_up_pow2(size, page_size) + page_size;

    void *head = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0);
    if (head == MAP_FAILED) {
        errno = ENOMEM;
        return NULL;
    }
    *(size_t *)head = size;
    return (uint8_t *)head + page_size;
}

void KISSMALLOC_NAME(free)(void *ptr)
{
    const size_t page_size = page_size_get();
    const size_t page_offset = (size_t)(((uint8_t *)ptr - (uint8_t *)NULL) & (page_size - 1));

    if (page_offset != 0) {
        void *page_start = (uint8_t *)ptr - page_offset;
        struct bucket_t *bucket = (struct bucket_t *)page_start;
        if (!__sync_sub_and_fetch(&bucket->object_count, 1))
            cache_push(bucket_get_mine()->cache, bucket);
    }
    else if (ptr != NULL) {
        void *head = (uint8_t *)ptr - page_size;
        if (munmap(head, *(size_t *)head) == -1) abort();
    }
}

void *KISSMALLOC_NAME(calloc)(size_t number, size_t size)
{
    return KISSMALLOC_NAME(malloc)(number * size);
}

void *KISSMALLOC_NAME(realloc)(void *ptr, size_t size)
{
    if (ptr == NULL) return KISSMALLOC_NAME(malloc)(size);

    if (size == 0) {
        if (ptr != NULL) KISSMALLOC_NAME(free)(ptr);
        return NULL;
    }

    if (size <= KISSMALLOC_GRANULARITY) return ptr;

    const size_t page_size = page_size_get();
    size_t copy_size = page_size;
    size_t page_offset = (size_t)((uint8_t *)ptr - (uint8_t *)NULL) & (page_size - 1);

    if (page_offset > 0) {
        void *page_start = (uint8_t *)ptr - page_offset;
        struct bucket_t *bucket = (struct bucket_t *)page_start;
        const size_t size_estimate_1 = page_size - bucket->bytes_free - page_offset;
        const size_t size_estimate_2 = page_size - bucket->bytes_free - ((bucket->object_count - 1) << KISSMALLOC_GRANULARITY_SHIFT);
            // might not work cleanly when reallocating in a different thread
        copy_size = (size_estimate_1 < size_estimate_2) ? size_estimate_1 : size_estimate_2;
    }

    if (copy_size > size) copy_size = size;

    void *new_ptr = KISSMALLOC_NAME(malloc)(size);
    if (new_ptr == NULL) return NULL;

    memcpy(new_ptr, ptr, copy_size);

    KISSMALLOC_NAME(free)(ptr);

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
        *ptr = KISSMALLOC_NAME(malloc)(size);
        return (*ptr != NULL) ? 0 : ENOMEM;
    }

    const size_t page_size = page_size_get();

    if (alignment + size < page_size >> 1) {
        uint8_t *ptr_byte = (uint8_t *)KISSMALLOC_NAME(malloc)(alignment + size);
        if (ptr_byte != NULL) {
            size_t r = (size_t)(ptr_byte - (uint8_t *)NULL) & (alignment - 1);
            if (r > 0) ptr_byte += alignment - r;
            *ptr = ptr_byte;
            return 0;
        }
    }

    size += alignment + page_size;

    void *head = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0);
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

void *KISSMALLOC_NAME(aligned_alloc)(size_t alignment, size_t size)
{
    void *ptr = NULL;
    KISSMALLOC_NAME(posix_memalign)(&ptr, alignment, size);
    return ptr;
}

void *KISSMALLOC_NAME(memalign)(size_t alignment, size_t size)
{
    void *ptr = NULL;
    KISSMALLOC_NAME(posix_memalign)(&ptr, alignment, size);
    return ptr;
}

void *KISSMALLOC_NAME(valloc)(size_t size)
{
    return KISSMALLOC_NAME(malloc)(round_up_pow2(size, page_size_get()));
}

void *KISSMALLOC_NAME(pvalloc)(size_t size)
{
    return KISSMALLOC_NAME(malloc)(round_up_pow2(size, page_size_get()));
}
