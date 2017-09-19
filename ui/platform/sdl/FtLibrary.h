/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Mutex>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_SYSTEM_H

namespace cc { template<class> class Singleton; }

namespace cc {
namespace ui {

class FtLibrary: public Mutex {
public:
    static FtLibrary *instance();

    FT_Library library() const { return library_; }

private:
    friend class Singleton<FtLibrary>;

    FtLibrary();
    ~FtLibrary();

    static void *alloc(FT_Memory  memory, long size);
    static void free(FT_Memory memory, void *block);
    static void *realloc(FT_Memory memory, long currentSize, long newSize, void *block);

    FT_Library library_;
    FT_Memory memory_;
    struct FT_MemoryRec_ memoryRec_;
};

class FtGuard {
public:
    FtGuard() { locked_ = FtLibrary::instance()->tryAcquire(); }
    ~FtGuard() { if (locked_) FtLibrary::instance()->release(); }
private:
    bool locked_;
};

}} // namespace cc::ui

