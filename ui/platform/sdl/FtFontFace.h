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
#include FT_TRUETYPE_TABLES_H

#include <cc/ui/FontFace>

namespace cc {
namespace ui {

class FtLibrary;
class FtGlyphCache;

class FtFontFace;
typedef List< Ref<FtFontFace> > FtFaceList;

class FtFontFace: public FontFace {
public:
    static Ref<FtFontFace> open(String path, int index = 0) { return new FtFontFace(path, index); }
    static Ref<FtFontFace> copy(FtFontFace *other) { return new FtFontFace(other); }

    inline String path() const { return path_; }
    inline int index() const { return index_; }

    inline FT_Face face() const { return face_; }

    inline String family() const { return face_->family_name; }
    inline bool fixedWidth() const { return face_->face_flags & FT_FACE_FLAG_FIXED_WIDTH; }
    inline bool italic() const { return face_->style_flags & FT_STYLE_FLAG_ITALIC; }
    Font::Stretch stretch() const;
    Font::Weight weight() const;

    inline int glyphCount() const { return face_->num_glyphs; }
    inline String styleName() const { return face_->style_name; }

    inline double ascender() const { return face_->size->metrics.ascender / 64.; }
    inline double descender() const { return face_->size->metrics.descender / 64.; }
    inline double lineHeight() const { return face_->size->metrics.height / 64.; }

    enum LoadFlags {
        GlyphHinting = 1,
        GlyphCaching = 2
    };

    bool loadGlyph(uchar_t ch, FT_Glyph *glyph, int flags = 0);

private:
    FtFontFace(String path, int index);
    FtFontFace(FtFontFace *other);
    ~FtFontFace();
    void loadFace();

    Ref<FtLibrary> ft_;
    String path_;
    int index_;
    FT_Face face_;
    TT_OS2 *os2_;

    Ref<FtGlyphCache> cache_;
    bool cacheIsHinted_;
};

}} // namespace cc::ui

