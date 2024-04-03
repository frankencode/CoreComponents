/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/FtTextRun>
#include <cc/FtFontManager>
#include <cc/FtTextCursor>

namespace cc {

FtTextRun FtTextRun::State::self() const
{
    return Object::alias<FtTextRun>(this);
}

void FtTextRun::State::append(const String &text, const Font &font)
{
    FtFontManager{}.selectFontRanges(text, font,
        [this, text](const Font &font, long i0, long i1) {
            String chunk = (i0 == 0 && i1 == text.count()) ? text : text.copy(i0, i1);

            FtGlyphRun run = FtFontManager{}.typeset(chunk, font, advance_).as<FtGlyphRun>();
            glyphRuns_.append(run);

            if (run.advance()[1] == 0) {
                if (firstLineHeight_ < run.size()[1])
                    size_[1] = firstLineHeight_ = run.size()[1];
            }
            else
                size_[1] = firstLineHeight_ + run.advance()[1];
            if (size_[0] < run.advance()[0]) size_[0] = run.advance()[0];

            if (run.maxAscender() > maxAscender_) maxAscender_ = run.maxAscender();
            if (run.minDescender() < minDescender_) minDescender_ = run.minDescender();

            advance_ = run.advance();
            byteCount_ += chunk.count();
        }
    );
}

TextRun FtTextRun::State::wrap(double maxWidth, TextAlign textAlign, double lineHeight, const TextWrapBehind &wrapBehind) const
{
    if (advance_[0] <= maxWidth) return self();

    if (lineHeight <= 0) {
        lineHeight = maxLineHeight(glyphRuns_);
        lineHeight = std::ceil(lineHeight); // FIXME rounding up for better pixel hinting
    }

    FtGlyphRun metaBlock = fold(glyphRuns_);
    metaBlock = metaBlock.wrap(maxWidth, textAlign, lineHeight, wrapBehind).as<FtGlyphRun>();
    return unfold(metaBlock, glyphRuns_);
}

double FtTextRun::State::maxLineHeight(const List<FtGlyphRun> &glyphRuns)
{
    double max = 0;
    for (const FtGlyphRun &glyphRun: glyphRuns) {
        double h = glyphRun.metrics().lineHeight();
        if (max < h) max = h;
    }
    return max;
}

FtGlyphRun FtTextRun::State::fold(const List<FtGlyphRun> &glyphRuns) const
{
    if (glyphRuns.count() == 1) return glyphRuns[0];

    FtGlyphRun metaBlock{new FtGlyphRun::State};
    FtGlyphRun::State &metaState = metaBlock.me();

    {
        List<String> parts;
        for (const FtGlyphRun &glyphRun: glyphRuns) {
            parts.append(glyphRun.text());
        }
        metaState.text_ = parts.join();
    }

    {
        int n = 0;
        int m = 0;
        for (const FtGlyphRun &glyphRun: glyphRuns) {
            n += glyphRun.me().cairoGlyphs_.count();
            m += glyphRun.me().cairoTextClusters_.count();
        }
        metaState.cairoGlyphs_ = FtGlyphRun::CairoGlyphs::allocate(n);
        metaState.cairoTextClusters_ = FtGlyphRun::CairoTextClusters::allocate(m);
        metaState.glyphAdvances_ = FtGlyphRun::GlyphAdvances::allocate(n);
    }
    {
        int j = 0;
        int k = 0;
        for (const FtGlyphRun &glyphRun: glyphRuns) {
            for (int i = 0, n = glyphRun.me().cairoGlyphs_.count(); i < n; ++i) {
                metaState.cairoGlyphs_[j] = glyphRun.me().cairoGlyphs_[i];
                metaState.glyphAdvances_[j] = glyphRun.me().glyphAdvances_[i];
                ++j;
            }
            for (int i = 0, m = glyphRun.me().cairoTextClusters_.count(); i < m; ++i) {
                metaState.cairoTextClusters_[k] = glyphRun.me().cairoTextClusters_[i];
                ++k;
            }
        }
    }

    if (glyphRuns.count() > 0) {
        const FtGlyphRun::State &firstState = glyphRuns[0].me();
        const FtGlyphRun::State &lastState = glyphRuns[glyphRuns.count() - 1].me();
        metaState.font_ = lastState.font_;
        metaState.origin_ = firstState.origin_;
        metaState.advance_ = lastState.advance_;
        metaState.size_[0] = lastState.advance_[0];
        for (const FtGlyphRun &glyphRun: glyphRuns) {
            if (metaState.size_[1] < glyphRun.me().size_[1])
                metaState.size_[1] = glyphRun.me().size_[1];
        }
        metaState.finalGlyphAdvance_ = lastState.finalGlyphAdvance_;
    }

    metaState.maxAscender_ = maxAscender_;
    metaState.minDescender_ = minDescender_;

    return metaBlock;
}

FtTextRun FtTextRun::State::unfold(FtGlyphRun &metaBlock, const List<FtGlyphRun> &glyphRuns) const
{
    FtTextRun textRun{new State};
    FtTextRun::State &textState = textRun.me();
    textState.advance_ = metaBlock.me().advance_;
    textState.size_ = metaBlock.me().size_;
    textState.maxAscender_ = metaBlock.me().maxAscender_;
    textState.minDescender_ = metaBlock.me().minDescender_;
    textState.byteCount_ = metaBlock.me().text_.count();
    textState.lineCount_ = metaBlock.me().lineCount_;

    int j = 0;
    int k = 0;
    for (const FtGlyphRun &glyphRun: glyphRuns)
    {
        FtGlyphRun newGlyphRun{new FtGlyphRun::State};
        newGlyphRun.me().text_ = glyphRun.me().text_;
        newGlyphRun.me().font_ = glyphRun.me().font_;
        newGlyphRun.me().origin_ = glyphRun.me().origin_;
        {
            int n = glyphRun.me().cairoGlyphs_.count();
            newGlyphRun.me().cairoGlyphs_ = metaBlock.me().cairoGlyphs_.select(j, j + n);
            j += n;
        }
        {
            int m = glyphRun.me().cairoTextClusters_.count();
            newGlyphRun.me().cairoTextClusters_ = metaBlock.me().cairoTextClusters_.select(k, k + m);
            k += m;
        }
        newGlyphRun.me().glyphAdvances_ = glyphRun.me().glyphAdvances_;
        newGlyphRun.me().finalGlyphAdvance_ = glyphRun.me().finalGlyphAdvance_;
        if (metaBlock.me().cairoGlyphs_.has(j)) {
            const cairo_glyph_t *glyph = &metaBlock.me().cairoGlyphs_[j];
            newGlyphRun.me().advance_ = Point{glyph->x, glyph->y};
        }
        else
            newGlyphRun.me().advance_ = metaBlock.me().advance_;

        newGlyphRun.me().scaledFont_ = glyphRun.me().scaledFont_;

        textRun.me().glyphRuns_.append(newGlyphRun);
    }

    return textRun;
}

TextCursor FtTextRun::State::getTextCursor(int offset) const
{
    if (byteCount_ < offset || offset < 0 || glyphRuns_.count() == 0)
        return TextCursor{};

    FtTextCursor cursor{self()};
    FtTextCursor::State &cursorState = cursor.me();

    if (offset == byteCount_) {
        const int runIndex = glyphRuns_.count() - 1;
        const FtGlyphRun &run = glyphRuns_[runIndex];
        cursorState.offset_ = offset;
        cursorState.runIndex_ = runIndex;
        cursorState.clusterIndex_ = run.me().cairoTextClusters_.count();
        cursorState.glyphIndex_ = run.me().cairoGlyphs_.count();
        cursorState.posA_ = Point { advance_[0], advance_[1] - run.metrics().ascender()  };
        cursorState.posB_ = Point { advance_[0], advance_[1] - run.metrics().descender() };
        return move(cursor);
    }

    int byteOffset0 = 0;

    for (int runIndex = 0; runIndex < glyphRuns_.count(); ++runIndex)
    {
        const FtGlyphRun &run = glyphRuns_[runIndex];
        int byteOffset1 = byteOffset0 + run.byteCount();

        if (offset < byteOffset1)
        {
            int glyphIndex = 0;

            for (int clusterIndex = 0; clusterIndex < run.me().cairoTextClusters_.count(); ++clusterIndex)
            {
                const cairo_text_cluster_t &cluster = run.me().cairoTextClusters_[clusterIndex];
                byteOffset1 = byteOffset0 + cluster.num_bytes;
                if (offset < byteOffset1) {
                    cursorState.offset_ = byteOffset0;
                    cursorState.runIndex_ = runIndex;
                    cursorState.clusterIndex_ = clusterIndex;
                    cursorState.glyphIndex_ = glyphIndex;
                    const cairo_glyph_t &glyph = run.me().cairoGlyphs_[glyphIndex];
                    cursorState.posA_ = Point { glyph.x, glyph.y - run.metrics().ascender()  };
                    cursorState.posB_ = Point { glyph.x, glyph.y - run.metrics().descender() };
                    return move(cursor);
                }

                byteOffset0 = byteOffset1;
                glyphIndex += cluster.num_glyphs;
            }
        }

        byteOffset0 = byteOffset1;
    }

    return TextCursor{};
}

int FtTextRun::State::moveTextCursor(FtTextCursor &cursor, int steps) const
{
    if (steps == 0) return 0;

    int stepsMoved = 0;

    int runIndex = cursor.runIndex();
    int clusterIndex = cursor.clusterIndex();
    int glyphIndex = cursor.glyphIndex();
    int offset = cursor.offset();

    if (steps > 0)
    {
        if (cursor.offset() == byteCount_) return 0;
        if (clusterIndex == glyphRuns_[runIndex].me().cairoTextClusters_.count()) return 0;

        for (; steps > 0; --steps) {
            const FtGlyphRun &run = glyphRuns_[runIndex];
            const cairo_text_cluster_t &cluster = run.me().cairoTextClusters_[clusterIndex];
            offset += cluster.num_bytes;
            glyphIndex += cluster.num_glyphs;
            ++clusterIndex;
            ++stepsMoved;

            if (clusterIndex == run.me().cairoTextClusters_.count()) {
                if (runIndex < glyphRuns_.count() - 1) {
                    ++runIndex;
                    clusterIndex = 0;
                    glyphIndex = 0;
                }
                else break;
            }
        }
    }
    else if (steps < 0)
    {
        if (cursor.offset() == 0) return 0;

        for (; steps < 0; ++steps) {
            if (clusterIndex == 0) {
                if (runIndex > 0) {
                    --runIndex;
                    clusterIndex = glyphRuns_[runIndex].me().cairoTextClusters_.count() - 1;
                    glyphIndex = glyphRuns_[runIndex].me().cairoGlyphs_.count();
                }
                else break;
            }
            else
                --clusterIndex;

            const FtGlyphRun &run = glyphRuns_[runIndex];
            const cairo_text_cluster_t &cluster = run.me().cairoTextClusters_[clusterIndex];
            offset -= cluster.num_bytes;
            glyphIndex -= cluster.num_glyphs;
            ++stepsMoved;
        }
    }

    cursor.me().runIndex_ = runIndex;
    cursor.me().clusterIndex_ = clusterIndex;
    cursor.me().glyphIndex_ = glyphIndex;
    cursor.me().offset_ = offset;

    const FtGlyphRun &run = glyphRuns_[runIndex];

    if (glyphIndex < run.me().cairoGlyphs_.count()) {
        const cairo_glyph_t &glyph = run.me().cairoGlyphs_[glyphIndex];
        cursor.me().posA_ = Point { glyph.x, glyph.y - run.metrics().ascender()  };
        cursor.me().posB_ = Point { glyph.x, glyph.y - run.metrics().descender() };
    }
    else {
        cursor.me().posA_ = Point { advance_[0], advance_[1] - run.metrics().ascender()  };
        cursor.me().posB_ = Point { advance_[0], advance_[1] - run.metrics().descender() };
    }

    return stepsMoved;
}

Point FtTextRun::State::advance(const FtTextCursor &cursor) const
{
    const FtGlyphRun::CairoGlyphs &cairoGlyphs = glyphRuns_[cursor.runIndex()].me().cairoGlyphs_;
    if (cursor.glyphIndex() < cairoGlyphs.count()) {
        const cairo_glyph_t &glyph = cairoGlyphs[cursor.glyphIndex()];
        return Point { glyph.x, glyph.y };
    }

    return advance_;
}

} // namespace cc
