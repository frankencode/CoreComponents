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

Ref<FtFontFace> FtFontFace::open(const String &path)
{
    return (new FtFontFace{path})->open();
}

FtFontFace::FtFontFace(const String &path):
    ftLibrary_{FtLibrary::instance()},
    path_{path}
{}

FtFontFace::~FtFontFace()
{
    FT_Done_Face(face_);
}

FtFontFace *FtFontFace::open()
{
    FT_Error error = FT_New_Face(ftLibrary_->ftLibrary(), path_, 0, &face_);
    if (error != 0)
        CC_DEBUG_ERROR(Format{"Failed to load font face (path = \"%%\", index = %%)\n"} << path_ << 0);

    family_ = face_->family_name;

    TT_OS2 *os2 = (TT_OS2 *)FT_Get_Sfnt_Table(face_, ft_sfnt_os2);
    if (os2) weight_ = static_cast<Weight>(os2->usWeightClass / 100);
    else weight_ = (face_->style_flags & FT_STYLE_FLAG_BOLD) ? Weight::Bold : Weight::Normal;
    if (os2) stretch_ = static_cast<Stretch>(os2->usWidthClass);
    else stretch_ = Stretch::Normal;

    slant_ = (face_->style_flags & FT_STYLE_FLAG_ITALIC) ? Slant::Italic : Slant::Normal;
    pitch_ = (face_->face_flags & FT_FACE_FLAG_FIXED_WIDTH) ? Pitch::Fixed : Pitch::Variable;

    return this;
}

String FtFontFace::toString() const
{
    return Format{}
        << "FtFontFace {" << nl
        << "  family:  " << family_ << nl
        << "  weight:  " << weight_ << nl
        << "  slant:   " << slant_ << nl
        << "  stretch: " << stretch_ << nl
        << "  pitch:   " << pitch_ << nl
        << "  path:    " << path_ << nl
        << "}";
}

}} // namespace cc::ui
