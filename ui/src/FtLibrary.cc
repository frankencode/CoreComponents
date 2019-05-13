/*
 * Copyright (C) 2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_MODULE_H

#include <cc/exceptions>
// #include <cc/Memory>
#include <cc/Format>
#include <cc/ThreadLocalSingleton>
#include <cc/ui/FtLibrary>

namespace cc {
namespace ui {

FtLibrary *FtLibrary::instance()
{
    return ThreadLocalSingleton<FtLibrary>::instance();
}

FT_Library FtLibrary::ftLibrary()
{
    return library_;
}

FtLibrary::FtLibrary():
    memory_{&memoryRec_}
{
    memory_->user = 0;
    #if 0
    memory_->alloc = FtLibrary::alloc;
    memory_->free = FtLibrary::free;
    memory_->realloc = FtLibrary::realloc;
    FT_Error error = FT_New_Library(memory_, &library_);
    if (error != 0)
        CC_DEBUG_ERROR(Format{"Failed to initialize the freetype library (error = %%)"} << error);
    FT_Add_Default_Modules(library_); // FIXME: select needed modules specifically
    #endif
    FT_Error error = FT_Init_FreeType(&library_);
    if (error != 0)
        CC_DEBUG_ERROR(Format{"Failed to initialize the freetype library (error = %%)"} << error);
}

FtLibrary::~FtLibrary()
{
    FT_Done_FreeType(library_);
}

#if 0
void *FtLibrary::alloc(FT_Memory /*memory*/, long size)
{
    return Memory::allocate(size);
}

void FtLibrary::free(FT_Memory /*memory*/, void *block)
{
    Memory::free(block);
}

void *FtLibrary::realloc(FT_Memory /*memory*/, long currentSize, long newSize, void *block)
{
    void *newBlock = Memory::allocate(newSize);
    ::memcpy(newBlock, block, currentSize < newSize ? currentSize : newSize);
    Memory::free(block);
    return newBlock;
}
#endif

}} // namespace cc::ui
