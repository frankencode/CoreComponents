/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <sys/types.h>

namespace cc {

/** \brief memory allocator
  */
class Memory
{
public:
    static void *allocate(size_t size) throw();
    static void free(void *data) throw();

private:
    Memory();

    static Memory *instance() throw();

    class BucketHeader;

    size_t pageSize_;
    BucketHeader *bucket_;
};

} // namespace cc
