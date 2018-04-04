/*
 * Copyright (C) 2017-2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/FtTextRun>

namespace cc {
namespace ui {

FtTextRun::FtTextRun(FtTypeSetter *typeSetter):
    typeSetter_(typeSetter),
    glyphRuns_(FtGlyphRuns::create()),
    firstLineHeight_(0),
    byteCount_(0),
    textAlign_(TextAlign::Left)
{}

void FtTextRun::append(String text, const Font &font)
{
    if (!typeSetter_) return;
    Ref<const FtGlyphRun> run = typeSetter_->ftLayout(text, font);
    glyphRuns_->append(run);

    if (run->advance()[1] == 0) {
        Size size = run->size();
        if (firstLineHeight_ < size[1]) size_[1] = firstLineHeight_ = size[1];
    }
    else
        size_[1] = firstLineHeight_ + run->advance()[1];
    if (size_[0] < run->advance()[0]) size_[0] = run->advance()[0];

    advance_ = typeSetter_->advance();
    byteCount_ += text->count();
}

Ref<TextRun> FtTextRun::wrap(double maxWidth, double lineHeight, const TextWrapBehind &wrapBehind) const
{
    if (advance_[0] <= maxWidth) return const_cast<FtTextRun *>(this);

    if (lineHeight <= 0)
        lineHeight = maxLineHeight(glyphRuns_);

    Ref<const FtGlyphRun> metaBlock = fold(glyphRuns_);
    metaBlock = metaBlock->wrap(maxWidth, textAlign_, lineHeight, wrapBehind);
    return unfold(metaBlock, glyphRuns_);
}

double FtTextRun::maxLineHeight(const FtGlyphRuns *glyphRuns)
{
    double max = 0;
    for (const FtGlyphRun *glyphRun: glyphRuns) {
        FtFaceGuard guard(glyphRun->ftFont());
        FT_Face ftFace = guard->ftFace();
        double h = glyphRun->ftFont()->size() * ftFace->height / ftFace->units_per_EM;
        if (max < h) max = h;
    }
    return max;
}

Ref<const FtGlyphRun> FtTextRun::fold(const FtGlyphRuns *glyphRuns)
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
        metaBlock->cairoGlyphs_ = CairoGlyphs::create(n);
        metaBlock->cairoTextClusters_ = CairoTextClusters::create(m);
    }
    {
        int j = 0;
        int k = 0;
        for (const FtGlyphRun *glyphRun: glyphRuns) {
            for (int i = 0, n = glyphRun->cairoGlyphs_->count(); i < n; ++i) {
                metaBlock->cairoGlyphs_->at(j) = glyphRun->cairoGlyphs_->at(i);
                ++j;
            }
            for (int i = 0, m = glyphRun->cairoTextClusters_->count(); i < m; ++i) {
                metaBlock->cairoTextClusters_->at(k) = glyphRun->cairoTextClusters_->at(i);
                ++k;
            }
        }
    }

    if (glyphRuns->count() > 0) {
        const FtGlyphRun *lastRun = glyphRuns->at(glyphRuns->count() - 1);
        metaBlock->font_ = lastRun->font_;
        metaBlock->advance_ = lastRun->advance_;
        metaBlock->size_[0] = lastRun->advance_[0];
        metaBlock->finalGlyphAdance_ = lastRun->finalGlyphAdance_;
        for (const FtGlyphRun *glyphRun: glyphRuns) {
            if (metaBlock->size_[1] < glyphRun->size_[1])
                metaBlock->size_[1] = glyphRun->size_[1];
        }
    }

    return metaBlock;
}

Ref<FtTextRun> FtTextRun::unfold(const FtGlyphRun *metaBlock, const FtGlyphRuns *glyphRuns)
{
    Ref<FtTextRun> textRun = Object::create<FtTextRun>();
    textRun->advance_ = metaBlock->advance_;
    textRun->size_ = metaBlock->size_;
    textRun->byteCount_ = metaBlock->text_->count();
    textRun->lineCount_ = metaBlock->lineCount_;

    textRun->glyphRuns_ = FtGlyphRuns::create(glyphRuns->count());
    int j = 0;
    int k = 0;
    for (int i = 0; i < glyphRuns->count(); ++i) {
        const FtGlyphRun *glyphRun = glyphRuns->at(i);
        Ref<FtGlyphRun> newGlyphRun = Object::create<FtGlyphRun>(glyphRun->text_, glyphRun->font_);
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
        newGlyphRun->finalGlyphAdance_ = glyphRun->finalGlyphAdance_;
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

Ref<TextRun> FtTextRun::copy() const
{
    Ref<FtTextRun> textRun = new FtTextRun;
    textRun->typeSetter_ = typeSetter_->copy();
    textRun->glyphRuns_ = FtGlyphRuns::create(glyphRuns_->count());
    for (int i = 0, n = glyphRuns_->count(); i < n; ++i)
        textRun->glyphRuns_->at(i) = glyphRuns_->at(i)->ftCopy();
    textRun->advance_ = advance_;
    textRun->byteCount_ = byteCount_;
    return textRun;
}

Ref<TextRun> FtTextRun::copy(int byteOffset0, int byteOffset1) const
{
    Ref<FtTextRun> textRun = new FtTextRun;
    textRun->typeSetter_ = typeSetter_;
    textRun->glyphRuns_ = FtGlyphRuns::create();
    textRun->byteCount_ = 0;

    if (byteCount_ <= byteOffset0 || byteOffset1 <= 0 || byteOffset1 <= byteOffset0)
        return textRun;

    const FtGlyphRun *glyphRun0 = 0;
    double shiftX = 0;
    int blockOffset0 = 0;
    for (const FtGlyphRun *glyphRun: glyphRuns_) {
        int blockOffset1 = blockOffset0 + glyphRun->text()->count();
        if (!(
            (byteOffset1 <= blockOffset0) ||
            (blockOffset1 <= byteOffset0)
        )) {
            Ref<FtGlyphRun> glyphRun =
                glyphRun->copy(
                    byteOffset0 - blockOffset0,
                    byteOffset1 - blockOffset0
                );
            if (!glyphRun0) {
                glyphRun0 = glyphRun;
                if (glyphRun0->cairoGlyphs_->count() > 0)
                    shiftX = -glyphRun0->cairoGlyphs_->at(0).x;
            }
            if (shiftX != 0)
                FtGlyphRun::lineShift(glyphRun, shiftX);
            textRun->glyphRuns_->append(glyphRun);
        }
        blockOffset0 = blockOffset1;
    }

    if (textRun->glyphRuns_->count() > 0)
        textRun->advance_ = textRun->glyphRuns_->at(textRun->glyphRuns_->count() - 1)->advance();

    return textRun;
}

void FtTextRun::paste(int byteOffset0, int byteOffset1, const TextRun *textRun)
{
    if (byteOffset0 < 0) byteOffset0 = 0;
    else if (byteCount_ < byteOffset0) byteOffset0 = byteCount_;
    if (byteOffset1 < 0) byteOffset1 = 0;
    else if (byteCount_ < byteOffset1) byteOffset1 = byteCount_;
    if (byteOffset1 < byteOffset0) {
        int h = byteOffset0;
        byteOffset0 = byteOffset1;
        byteOffset1 = h;
    }

    if (byteOffset0 == byteCount_) {
        TextRun::append(textRun);
        return;
    }

    Ref<FtTextRun> textRun2 = FtTextRun::create(FtTypeSetter::create());

    int byteOffset = 0;
    for (const FtGlyphRun *glyphRun: glyphRuns_)
    {
        int nextByteOffset = byteOffset + glyphRun->text()->count();

        if (nextByteOffset <= byteOffset0 || byteOffset1 <= byteOffset) {
            textRun2->append(glyphRun->text(), glyphRun->font());
        }
        else if (byteOffset0 <= byteOffset && nextByteOffset <= byteOffset1) {
            ;
        }
        else {
            textRun2->append(
                glyphRun->text()->copy(
                    byteOffset0 - byteOffset,
                    byteOffset1 - byteOffset
                ),
                glyphRun->font()
            );
        }

        if (textRun2->byteCount_ == byteOffset0 && textRun) {
            for (const GlyphRun *pasteBlock: textRun->getAllGlyphRuns()) {
                textRun2->append(
                    pasteBlock->text(),
                    pasteBlock->font()
                );
            }
        }

        byteOffset = nextByteOffset;
    }

    typeSetter_ = textRun2->typeSetter_;
    glyphRuns_ = textRun2->glyphRuns_;
    advance_ = textRun2->advance_;
    byteCount_ = textRun2->byteCount_;
}

}} // namespace cc::ui
