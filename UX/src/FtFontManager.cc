/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/FtFontManager>
#include <cc/FtFontFace>
#include <cc/FtScaledFont>
#include <cc/FtGlyphRun>
#include <cc/FtTextRun>
#include <cc/Utf8>
#include <cc/Map>
#include <cc/CircularBuffer>

namespace cc {

struct FtFontManager::State final: public FontManager::State
{
    FontFace openFontFace(const String &path) override
    {
        return FtFontFace{path};
    }

    ScaledFont selectFont(const Font &font) const override
    {
        Font target = fixup(font);

        Locator pos;
        if (!fontCache_.find(target.family(), &pos)) {
            fontCache_.insert(target.family(), RecentFonts::allocate(6), &pos);
        }

        RecentFonts recentFonts = fontCache_.at(pos).value();

        for (int i = 0; i < recentFonts.fill(); ++i)
        {
            FtScaledFont candidate = recentFonts.back(i);
            if (
                candidate.font().size()           == target.size() &&
                candidate.font().slant()          == target.slant() &&
                candidate.font().weight()         == target.weight() &&
                candidate.font().stretch()        == target.stretch() &&
                candidate.font().smoothing()      == target.smoothing() &&
                candidate.font().outlineHinting() == target.outlineHinting() &&
                candidate.font().metricsHinting() == target.metricsHinting()
            )
                return move(candidate);
        }

        FtFontFace fontFace =
            selectFontFamily(target.family())
            .selectFontFace(target.weight(), target.slant(), target.stretch())
            .as<FtFontFace>();

        FtScaledFont scaledFont = FtScaledFont{fontFace, target};
        recentFonts.pushBack(scaledFont);

        return move(scaledFont);
    }

    GlyphRun typeset(const String &text, const Font &font, const Point &origin) const override
    {
        if (text.find('\n'))
            return typeset(text.replaced("\n", ""), font, origin);

        auto ftScaledFont = selectFont(font).as<FtScaledFont>();

        FtFaceGuard guard{ftScaledFont};
        FT_Face ftFace = guard.ftFace();

        FT_UInt32 glyphLoadingFlags = 0;
        {
            Font font = ftScaledFont.font();
            OutlineHinting hinting = font.outlineHinting();

            switch (font.smoothing()) {
                case FontSmoothing::Default:
                case FontSmoothing::Grayscale: {
                    if (hinting == OutlineHinting::Slight)
                        glyphLoadingFlags = FT_LOAD_TARGET_LIGHT;
                    else
                        glyphLoadingFlags = FT_LOAD_DEFAULT;
                    break;
                }
                case FontSmoothing::RgbSubpixel:
                case FontSmoothing::BgrSubpixel:
                    glyphLoadingFlags = FT_LOAD_TARGET_LCD;
                    break;
                case FontSmoothing::VrgbSubpixel:
                case FontSmoothing::VbgrSubpixel:
                    glyphLoadingFlags = FT_LOAD_TARGET_LCD_V;
                    break;
                case FontSmoothing::None:
                    glyphLoadingFlags = FT_LOAD_TARGET_MONO;
                    break;
                default:
                    glyphLoadingFlags = FT_LOAD_DEFAULT;
                    break;
            };
        }

        if (ftScaledFont.fontFace().pitch() == Pitch::Fixed) {
            // keep in sync with flags passed in cairo_ft_font_face_create_for_ft_face (e.g. in FtScaledFont)
            glyphLoadingFlags |= FT_LOAD_FORCE_AUTOHINT;
        }

        FtGlyphRun ftGlyphRun{text, font, origin};
        ftGlyphRun.me().scaledFont_ = ftScaledFont;

        const long codePointsCount = Utf8{text}.count();

        FtGlyphRun::CairoGlyphs cairoGlyphs = FtGlyphRun::CairoGlyphs::allocate(codePointsCount);
        FtGlyphRun::CairoTextClusters cairoTextClusters = FtGlyphRun::CairoTextClusters::allocate(codePointsCount);
        FtGlyphRun::GlyphAdvances glyphAdvances = FtGlyphRun::GlyphAdvances::allocate(codePointsCount);
        int cairoGlyphsCount = 0;
        int cairoTextClustersCount = 0;

        auto walker = Utf8{text}.begin();
        FT_UInt previousGlyphIndex = 0;

        Point pos = origin;
        Point pos0 = origin;

        Step glyphAdvance;

        for (;; ++cairoGlyphsCount)
        {
            uint32_t ch = 0;
            int byteCount = +walker;
            if (!walker) break;
            ch = *walker;
            ++walker;
            byteCount = +walker - byteCount;

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
                if (glyphIndex == 0) break;
                if (FT_Load_Glyph(ftFace, glyphIndex, glyphLoadingFlags) != 0) break;
            }

            cairoGlyphs[cairoGlyphsCount].index = glyphIndex;
            cairoGlyphs[cairoGlyphsCount].x = pos[0];
            cairoGlyphs[cairoGlyphsCount].y = pos[1];

            auto g = ftFace->glyph;
            auto a = &ftFace->glyph->advance;
            auto m = &ftFace->glyph->metrics;

            if (a->x == 0 && a->y == 0 && cairoTextClustersCount > 0) {
                cairoTextClusters[cairoTextClustersCount - 1].num_bytes += byteCount;
                cairoTextClusters[cairoTextClustersCount - 1].num_glyphs += 1;
            }
            else {
                cairoTextClusters[cairoTextClustersCount].num_bytes = byteCount;
                cairoTextClusters[cairoTextClustersCount].num_glyphs = 1;
                ++cairoTextClustersCount;
            }

            const double ascender = std::ceil(m->horiBearingY / 64.);
            const double descender = std::floor(ascender - m->height / 64.);
            if (ftGlyphRun.me().maxAscender_ < ascender) ftGlyphRun.me().maxAscender_ = ascender;
            if (ftGlyphRun.me().minDescender_ > descender) ftGlyphRun.me().minDescender_ = descender;

            glyphAdvance = Step {
                double(a->x + g->rsb_delta - g->lsb_delta),
                double(a->y)
            } / 64.;

            glyphAdvances[cairoGlyphsCount] = glyphAdvance[0];

            pos += glyphAdvance;

            previousGlyphIndex = glyphIndex;
        }

        {
            Step step = pos - pos0;

            const double maxGlyphHeight =
                std::ceil( ftFace->ascender * ftScaledFont.size() / ftFace->units_per_EM ) +
                std::ceil( -ftFace->descender * ftScaledFont.size() / ftFace->units_per_EM );

            Size size {
                std::abs(step[0]),
                std::abs(step[1]) + maxGlyphHeight
            };

            ftGlyphRun.me().advance_ = pos;
            ftGlyphRun.me().size_ = size;
        }

        cairoGlyphs.truncate(cairoGlyphsCount);
        cairoTextClusters.truncate(cairoTextClustersCount);
        glyphAdvances.truncate(cairoGlyphsCount);
        ftGlyphRun.me().cairoGlyphs_ = cairoGlyphs;
        ftGlyphRun.me().cairoTextClusters_ = cairoTextClusters;
        ftGlyphRun.me().glyphAdvances_ = glyphAdvances;
        ftGlyphRun.me().finalGlyphAdvance_ = glyphAdvance;

        return move(ftGlyphRun);
    }

    TextRun createTextRun() const override
    {
        return FtTextRun{};
    }

    void selectFontRanges(const String &text, const Font &font, const FontRange &fontRange) const override
    {
        bool allAscii = true;
        for (long i = 0, n = text.count(); i < n; ++i) {
            if (text.byteAt(i) >= 0x80) {
                allAscii = false;
                break;
            }
        }

        if (allAscii) {
            fontRange(font, 0, text.count());
            return;
        }

        auto ftScaledFont = selectFont(font).as<FtScaledFont>();
        Font targetFont = ftScaledFont.font();
        FtFontFace targetFontFace = ftScaledFont.ftFontFace();

        for (auto walker = Utf8{text}.begin(); walker;)
        {
            const auto start = +walker;

            uint32_t ch = 0;
            while (walker) {
                ch = *walker;
                if (FT_Get_Char_Index(targetFontFace.ftFace(), ch) == 0) break;
                ++walker;
            }

            if (start < +walker)
                fontRange(targetFont, start, +walker);

            if (!walker) break;

            bool searchExhausted = true;

            for (const FontFamily &fontFamily: fontFamilies_)
            {
                if (
                    fontFamily.name() == targetFont.family() ||
                    fontFamily.pitch() == Pitch::Fixed
                ) continue;

                FtFontFace fallbackFontFace =
                    fontFamily.selectFontFace(
                        targetFont.weight(),
                        targetFont.slant(),
                        targetFont.stretch()
                    ).as<FtFontFace>();

                if (FT_Get_Char_Index(fallbackFontFace.ftFace(), ch) == 0) continue;
                const auto start = +walker;
                ++walker;

                searchExhausted = false;

                for (;walker; ++walker) {
                    ch = *walker;
                    if (
                        FT_Get_Char_Index(targetFontFace.ftFace(), ch) != 0 ||
                        FT_Get_Char_Index(fallbackFontFace.ftFace(), ch) == 0
                    ) break;
                }

                Font fallbackFont = targetFont;
                fallbackFont.family(fallbackFontFace.family());
                fallbackFont.weight(fallbackFontFace.weight());
                fallbackFont.slant(fallbackFontFace.slant());
                fallbackFont.stretch(fallbackFontFace.stretch());
                fontRange(fallbackFont, start, +walker);
                break;
            }

            if (searchExhausted) {
                const auto start = +walker;
                ++walker;
                fontRange(targetFont, start, +walker);
            }
        }
    }

    using RecentFonts = CircularBuffer<FtScaledFont>;
    using FontCache = Map<String, RecentFonts>;

    mutable FontCache fontCache_;
};

FtFontManager::FtFontManager():
    FontManager{instance<State>()}
{}

FtFontManager::State &FtFontManager::me()
{
    return Object::me.as<State>();
}

const FtFontManager::State &FtFontManager::me() const
{
    return Object::me.as<State>();
}

} // namespace cc
