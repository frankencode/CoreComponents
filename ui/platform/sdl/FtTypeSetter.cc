/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <math.h>
#include <string.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_OUTLINE_H
#include FT_RENDER_H

#include <cc/stdio> // DEBUG
#include <cc/Unicode>
#include <cc/ui/sdl/FtLibrary>
#include <cc/ui/sdl/FtFontFace>
#include <cc/ui/sdl/FtActivation>
#include <cc/ui/sdl/FtTypeSetter>

namespace cc {
namespace ui {

FtTypeSetter::~FtTypeSetter()
{}

void FtTypeSetter::stage(TextBlock *block, double *penX, double *penY, Image *image, GlyphRun *run) const
{
    FtFontFace *face = 0;
    FtActivation activation(block->style()->font(), &face);

    bool useKerning = FT_HAS_KERNING(face->face());
    // fout("useKerning = %%\n") << useKerning;

    RasterArgs args;
    FT_Raster_Params params;

    if (image) {
        args.image = image;
        args.color = block->style()->font()->color();

        ::memset(&params, 0, sizeof(params));
        params.flags = FT_RASTER_FLAG_DIRECT | FT_RASTER_FLAG_AA;
        params.user = (void *)&args;
        params.gray_spans = &FtTypeSetter::drawSpans;
    }

    Ref<Unicode> source;
    if (!(image && run)) source = Unicode::open(block->text());
    FT_UInt previousIndex = 0;

    double x = penX ? *penX : 0.;
    double y = penY ? *penY : 0.;
    double x0 = x;
    double y0 = y;
    uchar_t ch = 0;

    for (int i = 0; true; ++i)
    {
        if (image && run) {
            if (!run->has(i)) break;
            GlyphStop *stop = run->at(i);
            ch = stop->ch;
            x = x0 + stop->penX;
            y = y0 + stop->penY;
        }
        else {
            if (!source->has(i)) break;
            bool hasKerning = ch > 0x20;
            ch = source->at(i);
            hasKerning = hasKerning && (ch > 0x20);
            if (useKerning && hasKerning) {
                FT_UInt glyphIndex = FT_Get_Char_Index(face->face(), ch); // FIXME: store glyphIndex in the glyph cache?
                if (useKerning && previousIndex != 0 && glyphIndex != 0) {
                    FT_Vector delta;
                    FT_Get_Kerning(face->face(), previousIndex, glyphIndex, block->style()->font()->hinting() ? FT_KERNING_DEFAULT : FT_KERNING_UNFITTED, &delta);
                    x += delta.x / 64.;
                }
                previousIndex = glyphIndex;
            }
        }

        FT_Glyph glyph = 0;
        while (!glyph) {
            if (ch >= 0x20) {
                face->loadGlyph(
                    ch, &glyph,
                    (block->style()->font()->hinting() ? FtFontFace::GlyphHinting : 0) |
                    (caching_ ? FtFontFace::GlyphCaching : 0)
                );
            }
            if (!glyph && ch != 0xFFFD) {
                ch = 0xFFFD;
                continue;
            }
            break;
        }

        if (image && glyph) {
            FT_Library library = FtLibrary::instance()->library();
            FT_OutlineGlyph outlineGlyph = (FT_OutlineGlyph)glyph;
            args.penX = x;
            args.penY = y;
            FT_Outline_Render(library, &outlineGlyph->outline, &params);
        }

        double advanceX = glyph ? glyph->advance.x / 65536. : 0.;
        double advanceY = glyph ? glyph->advance.y / 65536. : 0.;
        if (!image && run) {
            run->pushBack(
                GlyphStop::create(ch, source->offset(), x, y, advanceX, advanceY)
            );
        }
        x += advanceX;
        y += advanceY;
    }

    if (penX) *penX = x;
    if (penY) *penY = y;
}

void FtTypeSetter::drawSpans(int y, int spanCount, const FT_Span *spans, void *user)
{
    // fout("FtTypeSetter::drawSpans(): y = %%, count = %%\n") << y << count;
    RasterArgs *args = (RasterArgs *)user;
    Image *image = args->image;
    y = args->penY - y;

    Color c = args->color;
    uint32_t o = Color::alpha(c);
    o += (o == 0xFF);

    for (int i = 0; i < spanCount; ++i) {
        const FT_Span span = spans[i];
        if (o == 0x100) Color::alpha(c) = span.coverage;
        else Color::alpha(c) = (span.coverage * o) >> 8;
        image->drawSpan(args->penX + span.x, y, span.len, c);
    }
}

}} // namespace cc::ui
