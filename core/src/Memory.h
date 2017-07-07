/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <sys/types.h>
#include <cc/MinHeap>

#ifndef MALLOC_ALIGNMENT
#define MALLOC_ALIGNMENT (2 * sizeof(size_t))
#endif

#ifndef CC_MEM_GRANULARITY
#define CC_MEM_GRANULARITY MALLOC_ALIGNMENT //!< system memory granularity, e.g. XMMS movdqa requires 16
#endif

#ifndef CC_MEM_PAGE_PREALLOC
#define CC_MEM_PAGE_PREALLOC 16 //!< number of pages to preallocate
#endif

#ifndef CC_MEM_PAGE_HEAP
#define CC_MEM_PAGE_HEAP (2 * CC_MEM_PAGE_PREALLOC - 1)  //!< number of freed pages to cache at maximum
#endif

// #define CC_MEM_ACCOUNTING 1

namespace cc {

/** \internal
  * \class Memory Memory.h cc/Memory
  * \ingroup mman
  * \brief %Memory allocator
  */
class Memory
{
public:
    static void *allocate(size_t size) throw();
    static void free(void *data) throw();

private:
    Memory();
    ~Memory();

    static Memory *instance() throw();

    class BucketHeader;

    class PageHeap: private MinHeap<void *>
    {
    public:
        PageHeap();
        void reduceTo(int maxFill, size_t pageSize
        #ifdef CC_MEM_ACCOUNTING
            , bool accounting
        #endif
        );
        void pushPage(void *page, size_t pageSize
        #ifdef CC_MEM_ACCOUNTING
            , bool accounting
        #endif
        );

    private:
        void *buf_[CC_MEM_PAGE_HEAP];
    };

    size_t pageSize_;
    BucketHeader *bucket_;
    PageHeap pageHeap_;
    #ifdef CC_MEM_ACCOUNTING
    bool accounting_;
    #endif
};

} // namespace cc
