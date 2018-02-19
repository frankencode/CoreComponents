/*
 * Copyright (C) 2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_TRUETYPE_TABLES_H
#include <cc/exceptions>
#include <cc/Format>
#include <cc/ui/FtLibrary>
#include <cc/ui/FtFontFace>

namespace cc {
namespace ui {

Ref<FtFontFace> FtFontFace::open(String path)
{
    return (new FtFontFace(path))->open();
}

FtFontFace::FtFontFace(String path):
    ftLibrary_(FtLibrary::instance()),
    path_(path)
{}

FtFontFace::~FtFontFace()
{
    FT_Done_Face(face_);
}

FtFontFace *FtFontFace::open()
{
    FT_Error error = FT_New_Face(ftLibrary_->ftLibrary(), path_, 0, &face_);
    if (error != 0)
        CC_DEBUG_ERROR(Format("Failed to load font face (path = \"%%\", index = %%)\n") << path_ << 0);

    family_ = face_->family_name;

    TT_OS2 *os2 = (TT_OS2 *)FT_Get_Sfnt_Table(face_, ft_sfnt_os2);
    if (os2) weight_ = static_cast<Weight>(os2->usWeightClass);
    else weight_ = (face_->style_flags & FT_STYLE_FLAG_BOLD) ? Weight::Bold : Weight::Normal;
    stretch_ = getStretch(os2);

    slant_ = (face_->style_flags & FT_STYLE_FLAG_ITALIC) ? Slant::Italic : Slant::Normal;
    pitch_ = (face_->face_flags & FT_FACE_FLAG_FIXED_WIDTH) ? Pitch::Fixed : Pitch::Variable;

    return this;
}

Stretch FtFontFace::getStretch(const TT_OS2 *os2)
{
    if (os2) {
        switch (os2->usWidthClass) {
            case 1: return Stretch::UltraCondensed;
            case 2: return Stretch::ExtraCondensed;
            case 3: return Stretch::Condensed;
            case 4: return Stretch::SemiCondensed;
            case 5: return Stretch::Normal;
            case 6: return Stretch::SemiExpanded;
            case 7: return Stretch::Expanded;
            case 8: return Stretch::ExtraExpanded;
            case 9: return Stretch::UltraExpanded;
        };
    }
    return Stretch::Normal;
}

}} // namespace cc::ui
