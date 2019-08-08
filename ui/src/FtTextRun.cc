/*
 * Copyright (C) 2017-2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/FtFontManager>
#include <cc/ui/FtTextCursor>
#include <cc/ui/FtTextRun>

namespace cc {
namespace ui {

FtTextRun::FtTextRun():
    glyphRuns_{FtGlyphRuns::create()}
{}

Ref<TextRun> FtTextRun::copy() const
{
    Ref<FtTextRun> textRun = new FtTextRun;
    textRun->glyphRuns_ = FtGlyphRuns::create(glyphRuns_->count());
    for (int i = 0, n = glyphRuns_->count(); i < n; ++i)
        textRun->glyphRuns_->at(i) = glyphRuns_->at(i)->ftCopy();
    textRun->advance_ = advance_;
    textRun->firstLineHeight_ = firstLineHeight_;
    textRun->size_ = size_;
    textRun->maxAscender_ = maxAscender_;
    textRun->minDescender_ = minDescender_;
    textRun->lineCount_ = lineCount_;
    textRun->byteCount_ = byteCount_;
    return textRun;
}

void FtTextRun::append(const String &text, const Font &font)
{
    FtFontManager::instance()->selectFontRanges(text, font,
        [=](const Font &font, int i0, int i1) {
            String chunk = (i0 == 0 && i1 == text->count()) ? text : text->copy(i0, i1);

            Ref<const FtGlyphRun> run = FtFontManager::instance()->ftTypeset(chunk, font, advance_);
            glyphRuns_->append(run);

            if (run->advance()[1] == 0) {
                if (firstLineHeight_ < run->size()[1])
                    size_[1] = firstLineHeight_ = run->size()[1];
            }
            else
                size_[1] = firstLineHeight_ + run->advance()[1];
            if (size_[0] < run->advance()[0]) size_[0] = run->advance()[0];

            if (run->maxAscender() > maxAscender_) maxAscender_ = run->maxAscender();
            if (run->minDescender() < minDescender_) minDescender_ = run->minDescender();

            advance_ = run->advance();
            byteCount_ += chunk->count();
        }
    );
}

Ref<TextRun> FtTextRun::wrap(double maxWidth, double lineHeight, const TextWrapBehind &wrapBehind) const
{
    if (advance_[0] <= maxWidth) return const_cast<FtTextRun *>(this);

    if (lineHeight <= 0)
        lineHeight = std::ceil(maxLineHeight(glyphRuns_)); // FIXME rounding up for better pixel hinting

    Ref<const FtGlyphRun> metaBlock = fold(glyphRuns_);
    metaBlock = metaBlock->wrap(maxWidth, textAlign_, lineHeight, wrapBehind);
    return unfold(metaBlock, glyphRuns_);
}

Ref<TextCursor> FtTextRun::getTextCursor(int byteOffset) const
{
    if ( byteCount_ < byteOffset || byteOffset < 0)
        return Object::create<FtTextCursor>();

    Ref<FtTextCursor> cursor = Object::create<FtTextCursor>(this);

    if (byteOffset == byteCount()) {
        const int runIndex = glyphRuns_->count() - 1;
        const FtGlyphRun *run = glyphRuns_->at(runIndex);
        cursor->byteOffset_ = byteOffset;
        cursor->runIndex_ = runIndex;
        cursor->clusterIndex_ = run->cairoTextClusters_->count();
        cursor->glyphIndex_ = run->cairoGlyphs_->count();
        Ref<const FontMetrics> metrics = run->ftScaledFont()->metrics();
        cursor->posA_ = Point { advance()[0], advance()[1] - metrics->ascender()  };
        cursor->posB_ = Point { advance()[0], advance()[1] - metrics->descender() };
        return cursor;
    }

    int byteOffset0 = 0;

    for (int runIndex = 0; runIndex < glyphRuns_->count(); ++runIndex)
    {
        const FtGlyphRun *run = glyphRuns_->at(runIndex);
        int byteOffset1 = byteOffset0 + run->byteCount();

        if (byteOffset < byteOffset1)
        {
            int glyphIndex = 0;

            for (int clusterIndex = 0; clusterIndex < run->cairoTextClusters_->count(); ++clusterIndex)
            {
                const cairo_text_cluster_t *cluster = &run->cairoTextClusters_->at(clusterIndex);
                byteOffset1 = byteOffset0 + cluster->num_bytes;
                if (byteOffset < byteOffset1) {
                    cursor->byteOffset_ = byteOffset0;
                    cursor->runIndex_ = runIndex;
                    cursor->clusterIndex_ = clusterIndex;
                    cursor->glyphIndex_ = glyphIndex;
                    const cairo_glyph_t *glyph = &run->cairoGlyphs_->at(glyphIndex);
                    Ref<const FontMetrics> metrics = run->ftScaledFont()->metrics();
                    cursor->posA_ = Point { glyph->x, glyph->y - metrics->ascender()  };
                    cursor->posB_ = Point { glyph->x, glyph->y - metrics->descender() };
                    return cursor;
                }

                byteOffset0 = byteOffset1;
                glyphIndex += cluster->num_glyphs;
            }
        }

        byteOffset0 = byteOffset1;
    }

    return Object::create<FtTextCursor>();
}

Point FtTextRun::advance(const FtTextCursor *cursor) const
{
    const FtGlyphRun::CairoGlyphs *cairoGlyphs = glyphRuns_->at(cursor->runIndex_)->cairoGlyphs_;
    if (cursor->glyphIndex_ < cairoGlyphs->count()) {
        const cairo_glyph_t *glyph = &cairoGlyphs->at(cursor->glyphIndex_);
        return Point { glyph->x, glyph->y };
    }

    return advance_;
}

int FtTextRun::moveTextCursor(FtTextCursor *cursor, int steps) const
{
    if (steps == 0) return 0;

    int stepsMoved = 0;

    int runIndex = cursor->runIndex_;
    int clusterIndex = cursor->clusterIndex_;
    int glyphIndex = cursor->glyphIndex_;
    int byteOffset = cursor->byteOffset_;

    if (steps > 0)
    {
        if (cursor->byteOffset() == byteCount()) return 0;
        if (clusterIndex == glyphRuns_->at(runIndex)->cairoTextClusters_->count()) return 0;

        for (; steps > 0; --steps) {
            const FtGlyphRun *run = glyphRuns_->at(runIndex);
            const cairo_text_cluster_t *cluster = &run->cairoTextClusters_->at(clusterIndex);
            byteOffset += cluster->num_bytes;
            glyphIndex += cluster->num_glyphs;
            ++clusterIndex;
            ++stepsMoved;

            if (clusterIndex == run->cairoTextClusters_->count()) {
                if (runIndex < glyphRuns_->count() - 1) {
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
        if (cursor->byteOffset() == 0) return 0;

        for (; steps < 0; ++steps) {
            if (clusterIndex == 0) {
                if (runIndex > 0) {
                    --runIndex;
                    clusterIndex = glyphRuns_->at(runIndex)->cairoTextClusters_->count() - 1;
                    glyphIndex = glyphRuns_->at(runIndex)->cairoGlyphs_->count();
                }
                else break;
            }
            else
                --clusterIndex;

            const FtGlyphRun *run = glyphRuns_->at(runIndex);
            const cairo_text_cluster_t *cluster = &run->cairoTextClusters_->at(clusterIndex);
            byteOffset -= cluster->num_bytes;
            glyphIndex -= cluster->num_glyphs;
            ++stepsMoved;
        }
    }

    cursor->runIndex_ = runIndex;
    cursor->clusterIndex_ = clusterIndex;
    cursor->glyphIndex_ = glyphIndex;
    cursor->byteOffset_ = byteOffset;

    const FtGlyphRun *run = glyphRuns_->at(runIndex);
    Ref<const FontMetrics> metrics = run->ftScaledFont()->metrics();

    if (glyphIndex < run->cairoGlyphs_->count()) {
        const cairo_glyph_t *glyph = &run->cairoGlyphs_->at(glyphIndex);
        cursor->posA_ = Point { glyph->x, glyph->y - metrics->ascender()  };
        cursor->posB_ = Point { glyph->x, glyph->y - metrics->descender() };
    }
    else {
        cursor->posA_ = Point { advance()[0], advance()[1] - metrics->ascender()  };
        cursor->posB_ = Point { advance()[0], advance()[1] - metrics->descender() };
    }

    return stepsMoved;
}

double FtTextRun::maxLineHeight(const FtGlyphRuns *glyphRuns)
{
    double max = 0;
    for (const FtGlyphRun *glyphRun: glyphRuns) {
        FtFaceGuard guard{glyphRun->ftScaledFont()};
        FT_Face ftFace = guard->ftFace();
        double h = glyphRun->ftScaledFont()->size() * ftFace->height / ftFace->units_per_EM;
        if (max < h) max = h;
    }
    return max;
}

Ref<const FtGlyphRun> FtTextRun::fold(const FtGlyphRuns *glyphRuns) const
{
    if (glyphRuns->count() == 1) return glyphRuns->at(0);

    Ref<FtGlyphRun> metaBlock = Object::create<FtGlyphRun>();

    {
        Ref<StringList> parts = StringList::create(glyphRuns->count());
        for (int i = 0; i < glyphRuns->count(); ++i)
            parts->at(i) = glyphRuns->at(i)->text();
        metaBlock->text_ = parts->join();
    }

    {
        int n = 0;
        int m = 0;
        for (const FtGlyphRun *glyphRun: glyphRuns) {
            n += glyphRun->cairoGlyphs_->count();
            m += glyphRun->cairoTextClusters_->count();
        }
        metaBlock->cairoGlyphs_ = FtGlyphRun::CairoGlyphs::create(n);
        metaBlock->cairoTextClusters_ = FtGlyphRun::CairoTextClusters::create(m);
        metaBlock->glyphAdvances_ = FtGlyphRun::GlyphAdvances::create(n);
    }
    {
        int j = 0;
        int k = 0;
        for (const FtGlyphRun *glyphRun: glyphRuns) {
            for (int i = 0, n = glyphRun->cairoGlyphs_->count(); i < n; ++i) {
                metaBlock->cairoGlyphs_->at(j) = glyphRun->cairoGlyphs_->at(i);
                metaBlock->glyphAdvances_->at(j) = glyphRun->glyphAdvances_->at(i);
                ++j;
            }
            for (int i = 0, m = glyphRun->cairoTextClusters_->count(); i < m; ++i) {
                metaBlock->cairoTextClusters_->at(k) = glyphRun->cairoTextClusters_->at(i);
                ++k;
            }
        }
    }

    if (glyphRuns->count() > 0) {
        const FtGlyphRun *firstRun = glyphRuns->at(0);
        const FtGlyphRun *lastRun = glyphRuns->at(glyphRuns->count() - 1);
        metaBlock->font_ = lastRun->font_;
        metaBlock->origin_ = firstRun->origin_;
        metaBlock->advance_ = lastRun->advance_;
        metaBlock->size_[0] = lastRun->advance_[0];
        for (const FtGlyphRun *glyphRun: glyphRuns) {
            if (metaBlock->size_[1] < glyphRun->size_[1])
                metaBlock->size_[1] = glyphRun->size_[1];
        }
        metaBlock->finalGlyphAdvance_ = lastRun->finalGlyphAdvance_;
    }

    metaBlock->maxAscender_ = maxAscender_;
    metaBlock->minDescender_ = minDescender_;

    return metaBlock;
}

Ref<FtTextRun> FtTextRun::unfold(const FtGlyphRun *metaBlock, const FtGlyphRuns *glyphRuns) const
{
    Ref<FtTextRun> textRun = Object::create<FtTextRun>();
    textRun->advance_ = metaBlock->advance_;
    textRun->size_ = metaBlock->size_;
    textRun->maxAscender_ = metaBlock->maxAscender_;
    textRun->minDescender_ = metaBlock->minDescender_;
    textRun->byteCount_ = metaBlock->text_->count();
    textRun->lineCount_ = metaBlock->lineCount_;

    textRun->glyphRuns_ = FtGlyphRuns::create(glyphRuns->count());
    int j = 0;
    int k = 0;
    for (int i = 0; i < glyphRuns->count(); ++i) {
        const FtGlyphRun *glyphRun = glyphRuns->at(i);
        Ref<FtGlyphRun> newGlyphRun = Object::create<FtGlyphRun>(glyphRun->text_, glyphRun->font_, glyphRun->origin_);
        {
            int n = glyphRun->cairoGlyphs_->count();
            newGlyphRun->cairoGlyphs_ = metaBlock->cairoGlyphs_->select(j, j + n);
            j += n;
        }
        {
            int m = glyphRun->cairoTextClusters_->count();
            newGlyphRun->cairoTextClusters_ = metaBlock->cairoTextClusters_->select(k, k + m);
            k += m;
        }
        newGlyphRun->glyphAdvances_ = glyphRun->glyphAdvances_;
        newGlyphRun->finalGlyphAdvance_ = glyphRun->finalGlyphAdvance_;
        if (metaBlock->cairoGlyphs_->has(j)) {
            const cairo_glyph_t *glyph = &metaBlock->cairoGlyphs_->at(j);
            newGlyphRun->advance_ = Point{ glyph->x, glyph->y };
        }
        else
            newGlyphRun->advance_ = metaBlock->advance_;
        textRun->glyphRuns_->at(i) = newGlyphRun;
    }

    return textRun;
}

}} // namespace cc::ui
