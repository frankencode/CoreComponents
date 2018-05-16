/*
 * Copyright (C) 2017-2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/FtFontManager>
#include <cc/ui/FtGlyphRun>

namespace cc {
namespace ui {

FtGlyphRun::FtGlyphRun(String text, const Font &font):
    text_(text),
    font_(font)
{}

Ref<FtGlyphRun> FtGlyphRun::ftCopy() const
{
    Ref<FtGlyphRun> glyphRun = new FtGlyphRun(text_, font_);
    glyphRun->advance_ = advance_;
    glyphRun->size_ = size_;
    glyphRun->maxAscender_ = maxAscender_;
    glyphRun->minDescender_ = minDescender_;
    glyphRun->cairoGlyphs_ = cairoGlyphs_->copy();
    glyphRun->cairoTextClusters_ = cairoTextClusters_->copy();
    glyphRun->finalGlyphAdvance_ = finalGlyphAdvance_;
    return glyphRun;
}

Ref<GlyphRun> FtGlyphRun::wrap(double maxWidth, TextAlign textAlign, double lineHeight, const TextWrapBehind &wrapBehind) const
{
    if (cairoGlyphs_->count() == 0)
        return const_cast<FtGlyphRun *>(this);

    if (advance_[0] <= maxWidth)
    {
        if (
            textAlign == TextAlign::Left ||
            textAlign == TextAlign::Justify
        )
            return const_cast<FtGlyphRun *>(this);
    }

    Ref<FtGlyphRun> ftGlyphRun = Object::create<FtGlyphRun>(text_, font_);

    ftGlyphRun->cairoGlyphs_ = CairoGlyphs::create(cairoGlyphs_->count());
    ftGlyphRun->cairoTextClusters_ = cairoTextClusters_;
    ftGlyphRun->finalGlyphAdvance_ = finalGlyphAdvance_;

    int glyphOffset = 0;
    int byteOffset = 0;

    int lineStartByteOffset = 0;
    int lineStartGlyphOffset = 0;
    int lineStartClusterIndex = 0;

    int wrapByteOffset = 0;
    int wrapGlyphOffset = 0;
    int wrapClusterIndex = 0;

    if (lineHeight <= 0) {
        FtFaceGuard guard(ftScaledFont());
        FT_Face ftFace = guard->ftFace();
        lineHeight = ftScaledFont()->size() * ftFace->height / ftFace->units_per_EM;
    }
    double trailingSpace = 0;
    double shiftX = 0;
    double shiftY = 0;

    for (int clusterIndex = 0; clusterIndex < cairoTextClusters_->count(); ++clusterIndex)
    {
        const cairo_text_cluster_t *cluster = &cairoTextClusters_->at(clusterIndex);

        int nextGlyphOffset = glyphOffset + cluster->num_glyphs;
        double clusterX0 = cairoGlyphs_->at(glyphOffset).x + shiftX;
        double clusterX1 = (nextGlyphOffset < cairoGlyphs_->count() ? cairoGlyphs_->at(nextGlyphOffset).x : advance()[0]) + shiftX;

        if (clusterX1 <= maxWidth + trailingSpace || wrapGlyphOffset == 0) {
            if (wrapBehind(text_, byteOffset, cluster->num_bytes)) {
                if (text_->byteAt(byteOffset) <= 0x20)
                    trailingSpace = clusterX1 - clusterX0;
                else
                    trailingSpace = 0;
                wrapByteOffset = byteOffset + cluster->num_bytes;
                wrapGlyphOffset = nextGlyphOffset;
                wrapClusterIndex = clusterIndex + 1;
            }
        }
        else if (0 < wrapGlyphOffset) {
            if (textAlign != TextAlign::Left) {
                if (textAlign == TextAlign::Justify) {
                    lineJustify(
                        ftGlyphRun, maxWidth + trailingSpace - cairoGlyphs_->at(wrapGlyphOffset).x - shiftX,
                        lineStartByteOffset, lineStartGlyphOffset, lineStartClusterIndex,
                        wrapByteOffset, wrapGlyphOffset, wrapClusterIndex
                    );
                }
                else if (textAlign == TextAlign::Right) {
                    lineShift(
                        ftGlyphRun,
                        lineStartGlyphOffset,
                        wrapGlyphOffset,
                        maxWidth + trailingSpace - cairoGlyphs_->at(wrapGlyphOffset).x - shiftX
                    );
                }
                else if (textAlign == TextAlign::Center) {
                    lineShift(
                        ftGlyphRun,
                        lineStartGlyphOffset,
                        wrapGlyphOffset,
                        (maxWidth + trailingSpace - cairoGlyphs_->at(wrapGlyphOffset).x - shiftX) / 2
                    );
                }
                lineStartByteOffset = wrapByteOffset;
                lineStartGlyphOffset = wrapGlyphOffset;
                lineStartClusterIndex = wrapClusterIndex;
            }
            shiftX = -cairoGlyphs_->at(wrapGlyphOffset).x;
            shiftY += lineHeight;
            for (int j = wrapGlyphOffset; j < glyphOffset; ++j) {
                ftGlyphRun->cairoGlyphs_->at(j).x = cairoGlyphs_->at(j).x + shiftX;
                ftGlyphRun->cairoGlyphs_->at(j).y = cairoGlyphs_->at(j).y + shiftY;
            }
            wrapGlyphOffset = 0;
            ++ftGlyphRun->lineCount_;
        }

        for (; glyphOffset < nextGlyphOffset; ++glyphOffset) {
            ftGlyphRun->cairoGlyphs_->at(glyphOffset) = cairoGlyphs_->at(glyphOffset);
            ftGlyphRun->cairoGlyphs_->at(glyphOffset).x += shiftX;
            ftGlyphRun->cairoGlyphs_->at(glyphOffset).y += shiftY;
        }

        byteOffset += cluster->num_bytes;
    }

    if (textAlign == TextAlign::Right) {
        lineShift(
            ftGlyphRun,
            lineStartGlyphOffset,
            glyphOffset,
            maxWidth - (advance()[0] + shiftX)
        );
    }
    else if (textAlign == TextAlign::Center) {
        lineShift(
            ftGlyphRun,
            lineStartGlyphOffset,
            glyphOffset,
            (maxWidth - (advance()[0] + shiftX)) / 2
        );
    }

    ftGlyphRun->advance_ = Step{
        ftGlyphRun->cairoGlyphs_->at(ftGlyphRun->cairoGlyphs_->count() - 1).x,
        ftGlyphRun->cairoGlyphs_->at(ftGlyphRun->cairoGlyphs_->count() - 1).y
    } + ftGlyphRun->finalGlyphAdvance_;

    ftGlyphRun->size_ = Size{maxWidth, shiftY + size_[1]};
    ftGlyphRun->maxAscender_ = maxAscender_;
    ftGlyphRun->minDescender_ = minDescender_;

    return ftGlyphRun;
}

void FtGlyphRun::lineJustify(
    FtGlyphRun *ftGlyphRun, double spaceDelta,
    int byteOffset0, int glyphOffset0, int clusterIndex0,
    int byteOffset1, int glyphOffset1, int clusterIndex1
)
{
    if (spaceDelta <= 0) return;

    while (0 < byteOffset1 && ftGlyphRun->text_->byteAt(byteOffset1 - 1) <= 0x20)
        --byteOffset1;

    {
        int spacesCount = 0;
        for (int k = byteOffset0; k < byteOffset1; ++k)
            spacesCount += (ftGlyphRun->text_->byteAt(k) <= 0x20);

        if (spacesCount == 0) return;

        spaceDelta /= spacesCount;
    }

    double shiftX = 0;
    int byteOffset = byteOffset0;
    int glyphOffset = glyphOffset0;

    for (int clusterIndex = clusterIndex0; clusterIndex < clusterIndex1; ++clusterIndex)
    {
        const cairo_text_cluster_t *cluster = &ftGlyphRun->cairoTextClusters_->at(clusterIndex);

        for (int j = 0; j < cluster->num_glyphs; ++j)
            ftGlyphRun->cairoGlyphs_->at(glyphOffset++).x += shiftX;

        if (ftGlyphRun->text_->byteAt(byteOffset) <= 0x20)
            shiftX += spaceDelta;

        byteOffset += cluster->num_bytes;
    }
}

void FtGlyphRun::lineShift(
    FtGlyphRun *ftGlyphRun,
    int glyphOffset0,
    int glyphOffset1,
    double shiftX
)
{
    for (int j = glyphOffset0; j < glyphOffset1; ++j)
        ftGlyphRun->cairoGlyphs_->at(j).x += shiftX;
}

void FtGlyphRun::lineShift(FtGlyphRun *ftGlyphRun, double shiftX)
{
    for (int j = 0, m = ftGlyphRun->cairoGlyphs_->count(); j < m; ++j)
        ftGlyphRun->cairoGlyphs_->at(j).x += shiftX;
}

Ref<GlyphRun> FtGlyphRun::elide(double maxWidth) const
{
    if (advance_[0] <= maxWidth) return const_cast<FtGlyphRun *>(this);

    Ref<const FtGlyphRun> ellipsis = FtFontManager::instance()->ftTypeset("...", font());
    maxWidth -= ellipsis->advance()[0];

    Ref<FtGlyphRun> ftGlyphRun = Object::create<FtGlyphRun>(text_, font());

    int byteOffset = 0;
    int glyphOffset = 0;

    for (int clusterIndex = 0; clusterIndex < cairoTextClusters_->count(); ++clusterIndex)
    {
        const cairo_text_cluster_t *cluster = &cairoTextClusters_->at(clusterIndex);
        int nextGlyphOffset = glyphOffset + cluster->num_glyphs;

        if (cairoGlyphs_->has(nextGlyphOffset) && maxWidth <= cairoGlyphs_->at(nextGlyphOffset).x)
        {
            ftGlyphRun->cairoGlyphs_ = CairoGlyphs::create(glyphOffset + ellipsis->cairoGlyphs_->count());

            for (int j = 0; j < glyphOffset; ++j)
                ftGlyphRun->cairoGlyphs_->at(j) = cairoGlyphs_->at(j);
            {
                double shiftX = cairoGlyphs_->at(glyphOffset).x;
                for (int j = 0; j < ellipsis->cairoGlyphs_->count(); ++j) {
                    ftGlyphRun->cairoGlyphs_->at(glyphOffset + j) = ellipsis->cairoGlyphs_->at(j);
                    ftGlyphRun->cairoGlyphs_->at(glyphOffset + j).x += shiftX;
                }
            }

            ftGlyphRun->cairoTextClusters_ = CairoTextClusters::create(clusterIndex + ellipsis->cairoTextClusters_->count());
            for (int i = 0; i < clusterIndex; ++i)
                ftGlyphRun->cairoTextClusters_->at(i) = cairoTextClusters_->at(i);
            for (int i = 0; i < ellipsis->cairoTextClusters_->count(); ++i)
                ftGlyphRun->cairoTextClusters_->at(clusterIndex + i) = ellipsis->cairoTextClusters_->at(i);

            ftGlyphRun->text_ = text_->copy(0, byteOffset) + ellipsis->text_;
            break;
        }

        byteOffset += cluster->num_bytes;
        glyphOffset = nextGlyphOffset;
    }

    ftGlyphRun->advance_ = Step{maxWidth, 0};
    ftGlyphRun->size_ = Size{maxWidth, size_[1]};
    ftGlyphRun->maxAscender_ = maxAscender_;
    ftGlyphRun->minDescender_ = minDescender_;

    return ftGlyphRun;
}

Ref<GlyphRun> FtGlyphRun::copy() const
{
    return ftCopy();
}

}} // namespace cc::ui
