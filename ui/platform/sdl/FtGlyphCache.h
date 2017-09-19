/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

#include <cc/types>
#include <cc/CircularBuffer>
#include <cc/Map>

namespace cc {
namespace ui {

class FtFontFace;

class FtGlyphCache: public Object
{
public:
    inline static FtGlyphCache *create(FtFontFace *face) {
        return new FtGlyphCache(face);
    }

    bool lookup(uchar_t ch, FT_Glyph *glyph) const;
    void insert(uchar_t ch, FT_Glyph glyph);
    void clear();

private:
    FtGlyphCache(FtFontFace *face);
    ~FtGlyphCache();

    void release();
    uint64_t cacheKey(uchar_t ch) const;

    typedef CircularBuffer<uint64_t> Queue;
    typedef Map<uint64_t, FT_Glyph> Cache;

    FtFontFace *face_;
    Ref<Queue> queue_;
    Ref<Cache> cache_;
};

}} // namespace cc::ui

