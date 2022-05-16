/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/FtGlyphRun>

namespace cc {

FtGlyphRun::State::State(const State &other):
    GlyphRun::State{other},
    cairoGlyphs_{other.cairoGlyphs_.copy()},
    cairoTextClusters_{other.cairoTextClusters_.copy()},
    glyphAdvances_{other.glyphAdvances_.copy()},
    finalGlyphAdvance_{other.finalGlyphAdvance_}
{}

GlyphRun FtGlyphRun::State::wrap(double maxWidth, TextAlign textAlign, double lineHeight, const TextWrapBehind &wrapBehind) const
{
    if (cairoGlyphs_.count() == 0) return self();

    if (advance_[0] <= maxWidth)
    {
        if (
            textAlign == TextAlign::Left ||
            textAlign == TextAlign::Justify
        )
            return self();
    }

    FtGlyphRun targetRun{new State{*this}};
    State &targetState = targetRun.me();

    int glyphOffset = 0;
    int byteOffset = 0;

    int lineStartByteOffset = 0;
    int lineStartGlyphOffset = 0;
    int lineStartClusterIndex = 0;

    int wrapByteOffset = 0;
    int wrapGlyphOffset = 0;
    int wrapClusterIndex = 0;

    if (lineHeight <= 0) lineHeight = std::round(targetRun.metrics().lineHeight());
    double trailingSpace = 0;
    double shiftX = 0;
    double shiftY = 0;

    for (int clusterIndex = 0; clusterIndex < cairoTextClusters_.count(); ++clusterIndex)
    {
        const cairo_text_cluster_t &cluster = cairoTextClusters_[clusterIndex];

        int nextGlyphOffset = glyphOffset + cluster.num_glyphs;
        double clusterX0 = cairoGlyphs_[glyphOffset].x + shiftX;
        double clusterX1 = (nextGlyphOffset < cairoGlyphs_.count() ? cairoGlyphs_[nextGlyphOffset].x : advance_[0]) + shiftX;

        if (clusterX1 <= maxWidth + trailingSpace || wrapGlyphOffset == 0) {
            if (wrapBehind(text_, byteOffset, cluster.num_bytes)) {
                if (text_.byteAt(byteOffset) <= 0x20)
                    trailingSpace = clusterX1 - clusterX0;
                else
                    trailingSpace = 0;
                wrapByteOffset = byteOffset + cluster.num_bytes;
                wrapGlyphOffset = nextGlyphOffset;
                wrapClusterIndex = clusterIndex + 1;
            }
        }
        else if (0 < wrapGlyphOffset) {
            if (textAlign != TextAlign::Left) {
                if (textAlign == TextAlign::Justify) {
                    targetState.lineJustify(
                        maxWidth + trailingSpace - cairoGlyphs_[wrapGlyphOffset].x - shiftX,
                        lineStartByteOffset, lineStartGlyphOffset, lineStartClusterIndex,
                        wrapByteOffset, wrapGlyphOffset, wrapClusterIndex
                    );
                }
                else if (textAlign == TextAlign::Right) {
                    targetState.lineShift(
                        lineStartGlyphOffset,
                        wrapGlyphOffset,
                        maxWidth + trailingSpace - cairoGlyphs_[wrapGlyphOffset].x - shiftX
                    );
                }
                else if (textAlign == TextAlign::Center) {
                    targetState.lineShift(
                        lineStartGlyphOffset,
                        wrapGlyphOffset,
                        (maxWidth + trailingSpace - cairoGlyphs_[wrapGlyphOffset].x - shiftX) / 2
                    );
                }
                lineStartByteOffset = wrapByteOffset;
                lineStartGlyphOffset = wrapGlyphOffset;
                lineStartClusterIndex = wrapClusterIndex;
            }
            shiftX = -cairoGlyphs_[wrapGlyphOffset].x;
            shiftY += lineHeight;
            for (int j = wrapGlyphOffset; j < glyphOffset; ++j) {
                targetState.cairoGlyphs_[j].x = cairoGlyphs_[j].x + shiftX;
                targetState.cairoGlyphs_[j].y = cairoGlyphs_[j].y + shiftY;
            }
            wrapGlyphOffset = 0;
            ++targetState.lineCount_;
        }

        for (; glyphOffset < nextGlyphOffset; ++glyphOffset) {
            targetState.cairoGlyphs_[glyphOffset].x += shiftX;
            targetState.cairoGlyphs_[glyphOffset].y += shiftY;
        }

        byteOffset += cluster.num_bytes;
    }

    if (textAlign == TextAlign::Right) {
        targetState.lineShift(
            lineStartGlyphOffset,
            glyphOffset,
            maxWidth - (advance_[0] + shiftX)
        );
    }
    else if (textAlign == TextAlign::Center) {
        targetState.lineShift(
            lineStartGlyphOffset,
            glyphOffset,
            (maxWidth - (advance_[0] + shiftX)) / 2
        );
    }

    targetState.advance_ = Step{
        targetState.cairoGlyphs_[targetState.cairoGlyphs_.count() - 1].x,
        targetState.cairoGlyphs_[targetState.cairoGlyphs_.count() - 1].y
    } + targetState.finalGlyphAdvance_;

    targetState.size_ = Size{maxWidth, shiftY + size_[1]};
    targetState.maxAscender_ = maxAscender_;
    targetState.minDescender_ = minDescender_;

    return move(targetRun);
}

void FtGlyphRun::State::lineJustify(
    double spaceDelta,
    int byteOffset0, int glyphOffset0, int clusterIndex0,
    int byteOffset1, int glyphOffset1, int clusterIndex1
)
{
    if (spaceDelta <= 0) return;

    while (0 < byteOffset1 && text_.byteAt(byteOffset1 - 1) <= 0x20)
        --byteOffset1;

    {
        int spacesCount = 0;
        for (int k = byteOffset0; k < byteOffset1; ++k)
            spacesCount += (text_.byteAt(k) <= 0x20);

        if (spacesCount == 0) return;

        spaceDelta /= spacesCount;
    }

    double shiftX = 0;
    int byteOffset = byteOffset0;
    int glyphOffset = glyphOffset0;

    for (int clusterIndex = clusterIndex0; clusterIndex < clusterIndex1; ++clusterIndex)
    {
        const cairo_text_cluster_t &cluster = cairoTextClusters_[clusterIndex];

        for (int j = 0; j < cluster.num_glyphs; ++j)
            cairoGlyphs_[glyphOffset++].x += shiftX;

        if (text_.byteAt(byteOffset) <= 0x20)
            shiftX += spaceDelta;

        byteOffset += cluster.num_bytes;
    }
}

void FtGlyphRun::State::lineShift(int glyphOffset0, int glyphOffset1, double shiftX)
{
    for (int j = glyphOffset0; j < glyphOffset1; ++j)
        cairoGlyphs_[j].x += shiftX;
}

void FtGlyphRun::State::lineShift(double shiftX)
{
    for (int j = 0, m = cairoGlyphs_.count(); j < m; ++j)
        cairoGlyphs_[j].x += shiftX;
}

GlyphRun FtGlyphRun::State::elide(double maxWidth) const
{
    if (advance_[0] <= maxWidth) return self();

    FtGlyphRun ellipsis = GlyphRun{"...", font_}.as<FtGlyphRun>();
    State &ellipsisState = ellipsis.me();

    maxWidth -= ellipsisState.advance_[0];

    if (maxWidth <= 0) {
        FtGlyphRun targetRun{"", font_, origin_};
        State &targetState = targetRun.me();
        targetState.size_ = Size{0, size_[1]};
        targetState.maxAscender_ = maxAscender_;
        targetState.minDescender_ = minDescender_;
        return move(targetRun);
    }

    FtGlyphRun targetRun{text_, font_, origin_};
    State &targetState = targetRun.me();

    int byteOffset = 0;
    int glyphOffset = 0;

    for (int clusterIndex = 0; clusterIndex < cairoTextClusters_.count(); ++clusterIndex)
    {
        const cairo_text_cluster_t &cluster = cairoTextClusters_[clusterIndex];
        int nextGlyphOffset = glyphOffset + cluster.num_glyphs;

        if (nextGlyphOffset < cairoGlyphs_.count() && maxWidth <= cairoGlyphs_[nextGlyphOffset].x)
        {
            targetState.cairoGlyphs_ = CairoGlyphs::allocate(glyphOffset + ellipsisState.cairoGlyphs_.count());
            targetState.glyphAdvances_ = GlyphAdvances::allocate(targetState.cairoGlyphs_.count());

            for (int j = 0; j < glyphOffset; ++j) {
                targetState.cairoGlyphs_[j] = cairoGlyphs_[j];
                targetState.glyphAdvances_[j] = glyphAdvances_[j];
            }
            {
                double shiftX = cairoGlyphs_[glyphOffset].x;
                for (int j = 0; j < ellipsisState.cairoGlyphs_.count(); ++j) {
                    targetState.cairoGlyphs_[glyphOffset + j] = ellipsisState.cairoGlyphs_[j];
                    targetState.cairoGlyphs_[glyphOffset + j].x += shiftX;
                    targetState.glyphAdvances_[glyphOffset + j] = ellipsisState.glyphAdvances_[j];
                }
            }

            targetState.cairoTextClusters_ = CairoTextClusters::allocate(clusterIndex + ellipsisState.cairoTextClusters_.count());
            for (int i = 0; i < clusterIndex; ++i)
                targetState.cairoTextClusters_[i] = cairoTextClusters_[i];
            for (int i = 0; i < ellipsisState.cairoTextClusters_.count(); ++i)
                targetState.cairoTextClusters_[clusterIndex + i] = ellipsisState.cairoTextClusters_[i];

            targetState.text_ = text_.copy(0, byteOffset) + ellipsisState.text_;
            break;
        }

        byteOffset += cluster.num_bytes;
        glyphOffset = nextGlyphOffset;
    }

    targetState.advance_ = Step{maxWidth, 0};
    targetState.size_ = Size{maxWidth, size_[1]};
    targetState.maxAscender_ = maxAscender_;
    targetState.minDescender_ = minDescender_;
    targetState.scaledFont_ = scaledFont_;

    return move(targetRun);
}

FtGlyphRun FtGlyphRun::State::self() const
{
    return Object::alias<FtGlyphRun>(this);
}

FtGlyphRun::FtGlyphRun(const String &text, const Font &font, const Point &origin):
    GlyphRun{new State{text, font, origin}}
{}

} // namespace cc
