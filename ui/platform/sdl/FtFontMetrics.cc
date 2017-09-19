/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_OUTLINE_H

#include <cc/assert>
#include <cc/ui/Application>
#include <cc/ui/sdl/FtLibrary>
#include <cc/ui/sdl/FtFontMetrics>

namespace cc {
namespace ui {

FtFontMetrics::FtFontMetrics(FtFontFace *face, double fontSize):
    face_(face),
    fontSize_(fontSize)
{
    FtGuard guard;
    FT_Error error = 0;
    error = FT_New_Size(face->face(), &size_);
    CC_ASSERT(error == 0);
    FT_Size oldSize = face->face()->size;
    error = FT_Activate_Size(size_);
    CC_ASSERT(error == 0);
    float xRes, yRes;
    Application::instance()->getDisplayResolution(&xRes, &yRes); // FIXME: inefficient
    error = FT_Set_Char_Size(face->face(), 0, fontSize * 64, xRes, yRes);
    CC_ASSERT(error == 0);
    error = FT_Activate_Size(oldSize);
    CC_ASSERT(error == 0);
}

FtFontMetrics::~FtFontMetrics()
{
    FtGuard guard;
    FT_Done_Size(size_);
}

double FtFontMetrics::capitalHeight() const
{
    FtGuard guard;
    double height = 0;
    FT_Glyph glyph = 0;
    if (face_->loadGlyph('M', &glyph, FtFontFace::GlyphCaching))
    {
        FT_OutlineGlyph outlineGlyph = (FT_OutlineGlyph)glyph;
        FT_Outline *outline = &outlineGlyph->outline;

        FT_BBox box;
        FT_Outline_Get_CBox(outline, &box);
        height = box.yMax / 64.;
    }

    return height;
}

}} // namespace cc::ui
