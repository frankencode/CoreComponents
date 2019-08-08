/*
 * Copyright (C) 2017-2018 Frank Mertens.
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
#include <cc/ThreadLocalSingleton>
#include <cc/ui/FtGlyphRun>
#include <cc/ui/FtTextRun>
#include <cc/ui/FtScaledFont>
#include <cc/ui/FtFontManager>

namespace cc {
namespace ui {

FtFontManager *FtFontManager::instance()
{
    return ThreadLocalSingleton<FtFontManager>::instance();
}

FtFontManager::FtFontManager():
    fontCache_{FontCache::create()}
{}

Ref<FontFace> FtFontManager::openFontFace(const String &path)
{
    return FtFontFace::open(path);
}

Ref<const ScaledFont> FtFontManager::selectFont(const Font &font) const
{
    Font target = fixup(font);

    Ref<RecentFonts> recentFonts;
    if (fontCache_->lookup(target->family(), &recentFonts)) {
        for (int i = 0; i < recentFonts->count(); ++i) {
            const FtScaledFont *candidate = recentFonts->at(i);
            if (
                candidate->font()->size() == target->size() &&
                candidate->font()->slant() == target->slant() &&
                candidate->font()->weight() == target->weight() &&
                candidate->font()->stretch() == target->stretch() &&
                candidate->font()->smoothing() == target->smoothing() &&
                candidate->font()->outlineHinting() == target->outlineHinting() &&
                candidate->font()->metricsHinting() == target->metricsHinting()
            )
                return candidate;
        }
    }
    else {
        recentFonts = RecentFonts::create(6);
            // buffer size is small to support scaling animations

        fontCache_->insert(target->family(), recentFonts);
    }

    const FtFontFace *fontFace =
        Object::cast<const FtFontFace *>(
            selectFontFamily(target->family())->selectFontFace(target->weight(), target->slant(), target->stretch())
        );

    Ref<const FtScaledFont> scaledFont = Object::create<FtScaledFont>(fontFace, target);
    recentFonts->pushBack(scaledFont);

    return scaledFont;
}

Ref<GlyphRun> FtFontManager::typeset(const String &text, const Font &font, const Point &origin) const
{
    return ftTypeset(text, font, origin);
}

Ref<FtGlyphRun> FtFontManager::ftTypeset(const String &text, const Font &font, const Point &origin) const
{
    if (text->contains('\n'))
        return ftTypeset(text->replace("\n", ""), font, origin);

    const FtScaledFont *ftScaledFont = Object::cast<const FtScaledFont *>(font->getScaledFont());

    FtFaceGuard guard{ftScaledFont};
    FT_Face ftFace = guard->ftFace();

    FT_UInt32 glyphLoadingFlags =
        [](const Font &f) -> FT_UInt32 {
            OutlineHinting h = f->outlineHinting();
            switch (f->smoothing()) {
                case FontSmoothing::Default:
                case FontSmoothing::Grayscale: {
                    if (h == OutlineHinting::Slight)
                        return FT_LOAD_TARGET_LIGHT;
                    else
                        return FT_LOAD_DEFAULT;
                }
                case FontSmoothing::RgbSubpixel:
                case FontSmoothing::BgrSubpixel:
                    return FT_LOAD_TARGET_LCD;
                case FontSmoothing::VrgbSubpixel:
                case FontSmoothing::VbgrSubpixel:
                    return FT_LOAD_TARGET_LCD_V;
                case FontSmoothing::None:
                    return FT_LOAD_TARGET_MONO;
            }
            return FT_LOAD_DEFAULT;
        }(ftScaledFont->font()) /*| FT_LOAD_FORCE_AUTOHINT*/;

    auto ftGlyphRun = Object::create<FtGlyphRun>(text, font, origin);

    const int codePointsCount = Utf8Walker::countCodePoints(text);

    auto cairoGlyphs = FtGlyphRun::CairoGlyphs::create(codePointsCount);
    auto cairoTextClusters = FtGlyphRun::CairoTextClusters::create(codePointsCount);
    auto glyphAdvances = FtGlyphRun::GlyphAdvances::create(codePointsCount);
    int cairoGlyphsCount = 0;
    int cairoTextClustersCount = 0;

    Utf8Walker walker(text);
    FT_UInt previousGlyphIndex = 0;

    Point pos = origin;
    Point pos0 = origin;

    Step glyphAdvance;

    for (;; ++cairoGlyphsCount)
    {
        uchar_t ch = 0;
        int byteCount = walker->offset();
        if (!walker->read(&ch)) break;

        byteCount = walker->offset() - byteCount;

        struct GlyphLoadingError {};

        FT_UInt glyphIndex = 0;

        try {
            glyphIndex = FT_Get_Char_Index(ftFace, ch);
            if (glyphIndex == 0) throw GlyphLoadingError{};

            if (FT_HAS_KERNING(ftFace)) {
                if (previousGlyphIndex != 0 && glyphIndex != 0 && ch > 0x20) {
                    FT_Vector delta;
                    if (FT_Get_Kerning(ftFace, previousGlyphIndex, glyphIndex, FT_KERNING_DEFAULT, &delta) == 0)
                        pos += Point{double(delta.x), double(delta.y)} / 64.;
                }
            }

            if (FT_Load_Glyph(ftFace, glyphIndex, glyphLoadingFlags) != 0) throw GlyphLoadingError{};
        }
        catch (GlyphLoadingError &) {
            glyphIndex = FT_Get_Char_Index(ftFace, 0xFFFD /* � */);
            if (glyphIndex == 0) continue;
            if (FT_Load_Glyph(ftFace, glyphIndex, glyphLoadingFlags) != 0) continue;
        }

        cairoGlyphs->at(cairoGlyphsCount).index = glyphIndex;
        cairoGlyphs->at(cairoGlyphsCount).x = pos[0];
        cairoGlyphs->at(cairoGlyphsCount).y = pos[1];

        auto g = ftFace->glyph;
        auto a = &ftFace->glyph->advance;
        auto m = &ftFace->glyph->metrics;

        if (a->x == 0 && a->y == 0 && cairoTextClustersCount > 0
        ) {
            cairoTextClusters->at(cairoTextClustersCount - 1).num_bytes += byteCount;
            cairoTextClusters->at(cairoTextClustersCount - 1).num_glyphs += 1;
        }
        else {
            cairoTextClusters->at(cairoTextClustersCount).num_bytes = byteCount;
            cairoTextClusters->at(cairoTextClustersCount).num_glyphs = 1;
            ++cairoTextClustersCount;
        }

        const double ascender = std::ceil(m->horiBearingY / 64.);
        const double descender = std::floor(ascender - m->height / 64.);
        if (ftGlyphRun->maxAscender_ < ascender) ftGlyphRun->maxAscender_ = ascender;
        if (ftGlyphRun->minDescender_ > descender) ftGlyphRun->minDescender_ = descender;

        glyphAdvance = Step {
            double(a->x + g->rsb_delta - g->lsb_delta),
            double(a->y)
        } / 64.;

        glyphAdvances->at(cairoGlyphsCount) = glyphAdvance[0];

        pos += glyphAdvance;

        previousGlyphIndex = glyphIndex;
    }

    {
        Step step = pos - pos0;

        const double maxGlyphHeight =
            std::ceil( ftFace->ascender * ftScaledFont->size() / ftFace->units_per_EM ) +
            std::ceil( -ftFace->descender * ftScaledFont->size() / ftFace->units_per_EM );

        Size size {
            std::abs(step[0]),
            std::abs(step[1]) + maxGlyphHeight
        };

        ftGlyphRun->advance_ = pos;
        ftGlyphRun->size_ = size;
    }

    cairoGlyphs->truncate(cairoGlyphsCount);
    cairoTextClusters->truncate(cairoTextClustersCount);
    glyphAdvances->truncate(cairoGlyphsCount);
    ftGlyphRun->cairoGlyphs_ = cairoGlyphs;
    ftGlyphRun->cairoTextClusters_ = cairoTextClusters;
    ftGlyphRun->glyphAdvances_ = glyphAdvances;
    ftGlyphRun->finalGlyphAdvance_ = glyphAdvance;

    return ftGlyphRun;
}

Ref<TextRun> FtFontManager::createTextRun() const
{
    return FtTextRun::create();
}

void FtFontManager::selectFontRanges(const String &text, const Font &font, const ReturnFontRange &fontRange) const
{
    bool allAscii = true;
    for (int i = 0, n = text->count(); i < n; ++i) {
        if (text->byteAt(i) >= 0x80) {
            allAscii = false;
            break;
        }
    }

    if (allAscii) {
        fontRange(font, 0, text->count());
        return;
    }

    Font targetFont = fixup(font);
    const FtFontFace *targetFontFace = Object::cast<const FtScaledFont *>(targetFont->getScaledFont())->ftFontFace();

    for (Utf8Walker walker{text}; walker;)
    {
        const int start = walker->offset();

        uchar_t ch = 0;
        while (walker) {
            ch = walker->getChar();
            if (FT_Get_Char_Index(targetFontFace->ftFace(), ch) == 0) break;
            ++walker;
        }

        if (start < walker->offset())
            fontRange(targetFont, start, walker->offset());

        if (!walker) break;

        bool searchExhausted = true;

        for (auto entry: fontFamilies_)
        {
            if (
                entry->key() == targetFont->family() ||
                entry->value()->pitch() == Pitch::Fixed
            ) continue;

            const FtFontFace *fallbackFontFace =
                Object::cast<const FtFontFace *>(
                    entry->value()->selectFontFace(
                        targetFont->weight(),
                        targetFont->slant(),
                        targetFont->stretch()
                    )
                );

            if (FT_Get_Char_Index(fallbackFontFace->ftFace(), ch) == 0) continue;
            const int start = walker->offset();
            ++walker;

            searchExhausted = false;

            while (walker) {
                ch = walker->getChar();
                if (
                    FT_Get_Char_Index(targetFontFace->ftFace(), ch) != 0 ||
                    FT_Get_Char_Index(fallbackFontFace->ftFace(), ch) == 0
                ) break;
                ++walker;
            }

            Font fallbackFont = targetFont;
            fallbackFont->setFamily (fallbackFontFace->family());
            fallbackFont->setWeight (fallbackFontFace->weight());
            fallbackFont->setSlant  (fallbackFontFace->slant());
            fallbackFont->setStretch(fallbackFontFace->stretch());
            fontRange(fallbackFont, start, walker->offset());
            break;
        }

        if (searchExhausted) {
            const int start = walker->offset();
            ++walker;
            fontRange(targetFont, start, walker->offset());
        }
    }
}

}} // namespace cc::ui
