/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/exceptions>
#include <cc/Format>
#include <cc/ui/sdl/FtLibrary>
#include <cc/ui/sdl/FtGlyphCache>
#include <cc/ui/sdl/FtFontFace>

namespace cc {
namespace ui {

FtFontFace::FtFontFace(String path, int index):
    path_(path),
    index_(index),
    cacheIsHinted_(false)
{
    loadFace();
}

FtFontFace::FtFontFace(FtFontFace *other):
    path_(other->path()),
    index_(other->index())
{
    loadFace();
}

FtFontFace::~FtFontFace()
{
    FtGuard guard;
    FT_Done_Face(face_);
}

void FtFontFace::loadFace()
{
    FtGuard guard;
    ft_ = FtLibrary::instance();
    FT_Error error = FT_New_Face(ft_->library(), path_, index_, &face_);
    if (error != 0)
        CC_DEBUG_ERROR(Format("Failed to load font face (path = \"%%\", index = %%)\n") << path_ << index_);

    os2_ = (TT_OS2 *)FT_Get_Sfnt_Table(face_, ft_sfnt_os2);
}

Font::Stretch FtFontFace::stretch() const
{
    Font::Stretch s = Font::Unstretched;
    if (os2_) {
        switch (os2_->usWidthClass) {
        case 1: { s = Font::UltraCondensed; break; }
        case 2: { s = Font::ExtraCondensed; break; }
        case 3: { s = Font::Condensed; break; }
        case 4: { s = Font::SemiCondensed; break; }
        case 5: { s = Font::Unstretched; break; }
        case 6: { s = Font::SemiExpanded; break; }
        case 7: { s = Font::Expanded; break; }
        case 8: { s = Font::ExtraExpanded; break; }
        case 9: { s = Font::UltraExpanded; break; }
        };
    }
    return s;
}

Font::Weight FtFontFace::weight() const
{
    if (os2_) return static_cast<Font::Weight>(os2_->usWeightClass);
    return face_->style_flags & FT_STYLE_FLAG_BOLD ? Font::Bold : Font::Normal;
}

bool FtFontFace::loadGlyph(uchar_t ch, FT_Glyph *glyph, int flags)
{
    bool caching = flags & GlyphCaching;
    bool hinting = flags & GlyphHinting;

    FtGuard guard;

    if (caching != (cache_ != 0)) {
        if (caching) cache_ = FtGlyphCache::create(this);
        else cache_ = 0;
    }

    if (cache_) {
        if (cacheIsHinted_ != hinting) {
            cache_->clear();
            cacheIsHinted_ = hinting;
        }
        else if (cache_->lookup(ch, glyph))
            return true;
    }

    FT_UInt glyphIndex = FT_Get_Char_Index(face_, ch);
    if (FT_Load_Glyph(face_, glyphIndex, hinting ? FT_LOAD_FORCE_AUTOHINT : (FT_LOAD_NO_AUTOHINT|FT_LOAD_NO_HINTING)) != 0)
        return false;

    CC_ASSERT(FT_Get_Glyph(face_->glyph, glyph) == 0);
    CC_ASSERT((*glyph)->format == FT_GLYPH_FORMAT_OUTLINE);

    if (cache_)
        cache_->insert(ch, *glyph);

    return true;
}

}} // namespace cc::ui
