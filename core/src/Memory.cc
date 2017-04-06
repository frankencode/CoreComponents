/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <sys/mman.h>
#include <unistd.h>
#include <assert.h>
#include <stdint.h>
#include <pthread.h>
#ifndef NDEBUG
#include <signal.h>
#endif
#include <errno.h>
#include <new>
#include <cc/Memory>

namespace cc {

#define CC_MEM_ALIGN(x) ((x) / CC_MEM_GRANULARITY + ((x) % CC_MEM_GRANULARITY > 0)) * CC_MEM_GRANULARITY

#ifndef NDEBUG
#define CC_MEM_ASSERT(x) \
    if (!(x)) { \
        sigset_t set; \
        sigemptyset(&set); \
        sigaddset(&set, SIGABRT); \
        pthread_sigmask(SIG_UNBLOCK, &set, 0); \
        raise(SIGABRT); \
    }
#else
#define CC_MEM_ASSERT(x)
#endif

#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS MAP_ANON
#endif

#ifndef MAP_POPULATE
#define MAP_POPULATE 0
#endif

#ifndef MAP_NORESERVE
#define MAP_NORESERVE 0
#endif

class Memory::BucketHeader
{
public:
    inline BucketHeader() { pthread_mutex_init(&mutex_, 0); }
    inline ~BucketHeader() { pthread_mutex_destroy(&mutex_); }
    inline void acquire() { pthread_mutex_lock(&mutex_); }
    inline void release() { pthread_mutex_unlock(&mutex_); }
    pthread_mutex_t mutex_; // FIXME: use a futex instead on Linux (saves 48 bytes!)
    uint16_t preallocCount_;
    uint16_t bytesDirty_;
    uint16_t objectCount_;
    bool open_;
};

Memory::PageHeap::PageHeap():
    MinHeap<void *>(buf_, CC_MEM_PAGE_HEAP)
{}

void Memory::PageHeap::reduceTo(int maxFill, size_t pageSize)
{
    if (count() <= maxFill) return;

    // reduce heap to maxFill and in the process unmap pages as sequential chunks
    void *chunk = pop();
    size_t size = pageSize;
    while (count() > maxFill) {
        void *chunk2 = pop();
        if ((char *)chunk2 - (char *)chunk == ssize_t(size)) {
            size += pageSize;
        }
        else {
            #ifndef NDEBUG
            int ret =
            #endif
            ::munmap(chunk, size);
            CC_MEM_ASSERT(ret == 0);
            chunk = chunk2;
            size = pageSize;
        }
    }
    if (size > 0) {
        #ifndef NDEBUG
        int ret =
        #endif
        ::munmap(chunk, size);
        CC_MEM_ASSERT(ret == 0);
    }
}

void Memory::PageHeap::pushPage(void *page, size_t pageSize)
{
    if (isFull()) reduceTo(CC_MEM_PAGE_HEAP / 2, pageSize);
    push(page);
}

Memory *Memory::instance() throw()
{
    static thread_local Memory instance_;
    return &instance_;
}

Memory::Memory():
    pageSize_(::sysconf(_SC_PAGE_SIZE)),
    bucket_(0)
{}

Memory::~Memory()
{
    pageHeap_.reduceTo(0, pageSize_);
}

void *Memory::allocate(size_t size) throw()
{
    Memory *allocator = instance();
    BucketHeader *bucket = allocator->bucket_;
    size_t pageSize = allocator->pageSize_;

    size = CC_MEM_ALIGN(size);

    if (size <= pageSize - CC_MEM_ALIGN(sizeof(BucketHeader)))
    {
        int preallocCount = 0;

        if (bucket)
        {
            bucket->acquire();
            if (size <= pageSize - bucket->bytesDirty_) {
                void *data = (void *)(((char *)bucket) + bucket->bytesDirty_);
                bucket->bytesDirty_ += size;
                ++bucket->objectCount_;
                bucket->release();
                return data;
            }
            else {
                bucket->open_ = false;
                bool dispose = (bucket->objectCount_ == 0);
                preallocCount = bucket->preallocCount_;
                bucket->release();
                if (dispose) {
                    bucket->~BucketHeader();
                    allocator->pageHeap_.pushPage((void *)bucket, pageSize);
                }
            }
        }

        void *pageStart = 0;
        if (preallocCount > 0) pageStart = (char *)bucket + pageSize;
        else pageStart = ::mmap(0, pageSize * CC_MEM_PAGE_PREALLOC, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE|MAP_NORESERVE|MAP_POPULATE, -1, 0);
        CC_MEM_ASSERT(pageStart != MAP_FAILED);
        bucket = new(pageStart)BucketHeader;
        CC_MEM_ASSERT((char*)bucket == (char*)pageStart);
        bucket->preallocCount_ = (preallocCount > 0) ? preallocCount - 1 : CC_MEM_PAGE_PREALLOC - 1;
        bucket->bytesDirty_ = CC_MEM_ALIGN(sizeof(BucketHeader)) + size;
        bucket->objectCount_ = 1;
        bucket->open_ = true;
        allocator->bucket_ = bucket;
        return (void *)((char *)pageStart + CC_MEM_ALIGN(sizeof(BucketHeader)));
    }

    if (size <= pageSize) {
        void *pageStart = ::mmap(0, pageSize, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE|MAP_NORESERVE|MAP_POPULATE, -1, 0);
        CC_MEM_ASSERT(pageStart != MAP_FAILED);
        return pageStart;
    }

    size += sizeof(uint32_t);
    uint32_t pageCount = size / pageSize + ((size % pageSize) > 0);
    void *pageStart = ::mmap(0, pageCount * pageSize, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE|MAP_NORESERVE/*|MAP_POPULATE*/, -1, 0);
    CC_MEM_ASSERT(pageStart != MAP_FAILED);
    *(uint32_t *)pageStart = pageCount;
    return (void *)((uint32_t *)pageStart + 1);
}

void Memory::free(void *data) throw()
{
    Memory *allocator = instance();
    if (!allocator) return; // FIXME

    size_t pageSize = allocator ? allocator->pageSize_ : size_t(::sysconf(_SC_PAGE_SIZE));

    uint32_t offset = ((char *)data - (char *)0) % pageSize;

    if (offset > sizeof(uint32_t)) {
        BucketHeader *bucket = (BucketHeader *)((char *)data - offset);
        bucket->acquire();
        bool dispose = ((--bucket->objectCount_) == 0) && !bucket->open_;
        bucket->release();
        if (dispose) {
            bucket->~BucketHeader();
            allocator->pageHeap_.pushPage((void *)bucket, pageSize);
        }
    }
    else if (offset == 0) {
        #ifndef NDEBUG
        int ret =
        #endif
        ::munmap(data, pageSize);
        CC_MEM_ASSERT(ret == 0);
    }
    else if (offset == sizeof(uint32_t)) {
        void *pageStart = (void *)((char *)data - sizeof(uint32_t));
        uint32_t pageCount = *(uint32_t *)pageStart;
        #ifndef NDEBUG
        int ret =
        #endif
        ::munmap(pageStart, pageCount * pageSize);
        CC_MEM_ASSERT(ret == 0);
    }
}

} // namespace cc
