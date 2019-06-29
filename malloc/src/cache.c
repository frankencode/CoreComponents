/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

typedef struct {
    void *buffer[CC_MEM_PAGE_CACHE];
    int fill;
} cache_t;

static thread_local cache_t thread_local_cache = { NULL, 0 };

inline static int cache_is_full(const cache_t *cache)
{
    return cache->fill == CC_MEM_PAGE_CACHE;
}

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

static void cache_reduce(cache_t *cache, int fill_max, size_t page_size)
{
    if (cache->fill <= fill_max) return;

    void *chunk = cache_pop(cache);
    size_t size = page_size;
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

inline static void cache_push(cache_t *cache, void *page, size_t page_size)
{
    if (cache_is_full(cache))
        cache_reduce(cache, CC_MEM_PAGE_CACHE >> 1, page_size);

    cache->buffer[cache->fill] = page;
    ++cache->fill;
    cache_bubble_up(cache);
}
