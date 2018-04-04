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
#include <cairo/cairo.h>
#include <cairo/cairo-ft.h>
#include <cmath>
#include <cc/Utf8Walker>
#include <cc/ui/FtGlyphRun>
#include <cc/ui/FtTextRun>
#include <cc/ui/FtTypeSetter>

namespace cc {
namespace ui {

Ref<FtGlyphRun> FtTypeSetter::ftLayout(String text, const Font &font)
{
    if (text->contains('\n'))
        return ftLayout(text->replace("\n", " ")->simplify(), font);

    const FtScaledFont *ftFont = Object::cast<const FtScaledFont *>(font->getScaledFont());
    FT_Face ftFace = cairo_ft_scaled_font_lock_face(ftFont->cairoScaledFont());

    Ref<FtGlyphRun> ftGlyphRun = Object::create<FtGlyphRun>(text, font);

    int codePointsCount = Utf8Walker::countCodePoints(text);

    Ref<CairoGlyphs> cairoGlyphs = CairoGlyphs::create(codePointsCount);
    Ref<CairoTextClusters> cairoTextClusters = CairoTextClusters::create(codePointsCount);
    int cairoGlyphsCount = 0;
    int cairoTextClustersCount = 0;

    Utf8Walker walker(text);
    FT_UInt previousGlyphIndex = 0;

    Point pos0 = pos_;
    double maxGlyphHeight = 0;

    Step glyphAdvance;

    while (true)
    {
        uchar_t ch = 0;
        int byteCount = walker->offset();
        if (!walker->read(&ch)) break;
        byteCount = walker->offset() - byteCount;

        struct GlyphLoadingError {};

        FT_UInt glyphIndex = 0;

        try {
            glyphIndex = FT_Get_Char_Index(ftFace, ch);
            if (glyphIndex == 0) throw GlyphLoadingError();

            if (FT_HAS_KERNING(ftFace)) {
                if (previousGlyphIndex != 0 && glyphIndex != 0 && ch > 0x20) {
                    FT_Vector delta;
                    if (FT_Get_Kerning(ftFace, previousGlyphIndex, glyphIndex, FT_KERNING_DEFAULT, &delta) == 0)
                        pos_ += Point{double(delta.x), double(delta.y)} / 64.;
                }
            }

            if (FT_Load_Glyph(ftFace, glyphIndex, FT_LOAD_DEFAULT) != 0) throw GlyphLoadingError();
        }
        catch (GlyphLoadingError &) {
            glyphIndex = FT_Get_Char_Index(ftFace, 0xFFFD /* � */);
            if (glyphIndex == 0) continue;
            if (FT_Load_Glyph(ftFace, glyphIndex, FT_LOAD_DEFAULT) != 0) continue;
        }

        cairoGlyphs->at(cairoGlyphsCount).index = glyphIndex;
        cairoGlyphs->at(cairoGlyphsCount).x = pos_[0];
        cairoGlyphs->at(cairoGlyphsCount).y = pos_[1];
        ++cairoGlyphsCount;

        if (
            ftFace->glyph->advance.x == 0 &&
            ftFace->glyph->advance.y == 0 &&
            cairoTextClustersCount > 0
        ) {
            cairoTextClusters->at(cairoTextClustersCount - 1).num_bytes += byteCount;
            cairoTextClusters->at(cairoTextClustersCount - 1).num_glyphs += 1;
        }
        else {
            cairoTextClusters->at(cairoTextClustersCount).num_bytes = byteCount;
            cairoTextClusters->at(cairoTextClustersCount).num_glyphs = 1;
            ++cairoTextClustersCount;
        }

        glyphAdvance = Step {
            double(ftFace->glyph->advance.x),
            double(ftFace->glyph->advance.y)
        } / 64.;

        pos_ += glyphAdvance;

        {
            double glyphHeight = ftFace->glyph->metrics.height / 64.;
            if (maxGlyphHeight < glyphHeight) maxGlyphHeight = glyphHeight;
        }

        previousGlyphIndex = glyphIndex;
    }

    {
        Step step = pos_ - pos0;
        Size size {
            std::abs(step[0]),
            std::abs(step[1]) + maxGlyphHeight
        };

        ftGlyphRun->advance_ = pos_;
        ftGlyphRun->size_ = size;
    }

    cairoGlyphs->truncate(cairoGlyphsCount);
    cairoTextClusters->truncate(cairoTextClustersCount);
    ftGlyphRun->cairoGlyphs_ = cairoGlyphs;
    ftGlyphRun->cairoTextClusters_ = cairoTextClusters;
    ftGlyphRun->finalGlyphAdance_ = glyphAdvance;

    cairo_ft_scaled_font_unlock_face(ftFont->cairoScaledFont());

    return ftGlyphRun;
}

Ref<TypeSetter> FtTypeSetter::copy() const
{
    Ref<FtTypeSetter> typeSetter = new FtTypeSetter;
    typeSetter->pos_ = pos_;
    return typeSetter;
}

Ref<TextRun> FtTypeSetter::createTextRun()
{
    return FtTextRun::create(this);
}


}} // namespace cc::ui
