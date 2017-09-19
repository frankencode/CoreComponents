/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ThreadLocalSingleton>
#include <cc/ui/sdl/FtLibrary>
#include <cc/ui/sdl/FtFontFace>
#include <cc/ui/sdl/FtGlyphCache>

namespace cc {
namespace ui {

FtGlyphCache::FtGlyphCache(FtFontFace *face):
    face_(face),
    queue_(Queue::create(1000)),
    cache_(Cache::create())
{}

FtGlyphCache::~FtGlyphCache()
{
    release();
}

void FtGlyphCache::release()
{
    for (int i = 0; i < cache_->count(); ++i)
        FT_Done_Glyph(cache_->valueAt(i));
}

void FtGlyphCache::clear()
{
    release();
    queue_->deplete();
    cache_->deplete();
}

uint64_t FtGlyphCache::cacheKey(uchar_t ch) const
{
    return
        (uint64_t(ch) << 32) |
        (uint64_t(face_->face()->size->metrics.x_ppem) << 16) |
        uint64_t(face_->face()->size->metrics.y_ppem);
}

bool FtGlyphCache::lookup(uchar_t ch, FT_Glyph *glyph) const
{
    return cache_->lookup(cacheKey(ch), glyph);
}

void FtGlyphCache::insert(uchar_t ch, FT_Glyph glyph)
{
    if (queue_->count() == queue_->capacity()) {
        uint64_t oldKey = queue_->popFront();
        int oldIndex = 0;
        FT_Glyph oldGlyph = 0;
        cache_->lookup(oldKey, &oldGlyph, &oldIndex);
        FT_Done_Glyph(oldGlyph);
        cache_->removeAt(oldIndex);
    }
    uint64_t key = cacheKey(ch);
    cache_->insert(key, glyph);
    queue_->pushBack(key);
}

}} // namespace cc::ui
