/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/TextRun>
#include <cc/ui/TextItem>
#include <cc/ui/TextLine>
#include <cc/ui/TextEdit>

namespace cc {
namespace ui {

TextEdit::TextEdit(View *parent):
    ItemView{parent, Item::create()},
    lines_{Lines::create()}
{
    font->bind([=]{ return app()->defaultFont(); });
    ink->bind([=]{ return theme()->primaryTextColor(); });
    paper->bind([=]{ return theme()->inputFieldFillColor(); });
    textWidth->bind([=]{ return size()[0] - lineNumberWidth(); });
    textWidthSansMargin->bind([=]{
        double w = textWidth() - 2 * textMargin()[0];
        if (w < 0) w = 0;
        return std::ceil(w);
    });

    lineNumberFont->bind([=]{ return app()->defaultFixedFont(); });
    lineNumberInk->bind([=]{ return theme()->secondaryTextColor(); });
    lineNumberPaper->bind([=]{ return theme()->windowColor(); });
    lineNumberWidth->bind([=]{
        return
            std::ceil(
                TextRun::create(String::create(lineNumberDigitCount(), '0'), lineNumberFont())->size()[0] +
                2 * lineNumberMargin()[0]
            );
    });

    lineNumberPos->bind([=]{
        return lineNumberMargin() + Point{ 0, std::ceil(lineNumberFont()->getMetrics()->ascender()) };
    });
    textPos->bind([=]{
        return textMargin() + Point{ 0, std::ceil(font()->getMetrics()->ascender()) };
    });

    lines_->insertAt(0, rootItem()->add<TextItem>(), 0);
}

String TextEdit::copy(Range range) const
{
    int lineIndex0 = 0;
    int lineIndex1 = 0;
    int currentByteOffset0 = 0;
    lines_->getView(range->i0(), range->i1(), &lineIndex0, &lineIndex1, &currentByteOffset0);

    Ref<StringList> parts = StringList::create();

    for (int lineIndex = lineIndex0; lineIndex < lineIndex1; ++lineIndex) {
        const TextItem *item = static_cast<TextItem *>(itemAt(lineIndex));
        parts->append(
            item->text()->copy(range->i0() - currentByteOffset0, range->i1() - currentByteOffset0)
        );
        currentByteOffset0 += item->text()->count();
    }

    return parts->join();
}

int TextEdit::count() const
{
    return lines_->extent();
}

Range TextEdit::paste(Range range, const String &text)
{
    cut(range);

    if (text->count() == 0) return Range{ range->i0(), range->i0() };

    TextItem *line = nullptr;
    int lineIndex = 0;
    int byteOffset0 = 0;
    if (!lines_->lookup(range->i0(), &line, &lineIndex, &byteOffset0))
        return Range{};

    String filteredText = filterNonPrintable(text);
    Ref<StringList> chunks = splitIntoLines(filteredText);
    int byteOffset = range->i0() - byteOffset0;

    for (const String &chunk: chunks) {
        if (chunk->endsWith('\n')) {
            String overhang = line->text()->copy(byteOffset, line->text()->count());
            line->text = line->text()->paste(byteOffset, line->text()->count(), chunk);
            lines_->updateExtentAt(lineIndex, line->text()->count());

            auto newLine = Object::create<TextItem>(overhang);
            ++lineIndex;
            rootItem()->insertAt(lineIndex, newLine);
            lines_->insertAt(lineIndex, newLine, newLine->text()->count());
            line = newLine;
            byteOffset = 0;
        }
        else {
            line->text = line->text()->paste(byteOffset, byteOffset, chunk);
            lines_->updateExtentAt(lineIndex, line->text()->count());
        }
    }

    return Range{range->i0(), range->i0() + filteredText->count()};
}

String TextEdit::filterNonPrintable(const String &text)
{
    String filteredText(text->count(), '\0');
    int j = 0;
    for (int i = 0; i < text->count(); ++i) {
        uint8_t ch = text->byteAt(i);
        if (ch >= 0x20 || ch == '\n') {
            mutate(filteredText)->byteAt(j) = ch;
            ++j;
        }
    }
    mutate(filteredText)->truncate(j);
    return filteredText;
}

Ref<StringList> TextEdit::splitIntoLines(const String &text)
{
    Ref<StringList> chunks = StringList::create();
    for (int i = 0; i < text->count();) {
        int i0 = i;
        i = text->find('\n', i);
        i += (i < text->count());
        chunks->append(text->copy(i0, i));
    }
    return chunks;
}

void TextEdit::cut(Range range)
{
    int lineIndex0 = 0;
    int lineIndex1 = 0;
    int currentByteOffset0 = 0;
    lines_->getView(range->begin(), range->end(), &lineIndex0, &lineIndex1, &currentByteOffset0);

    for (int lineIndex = lineIndex0; lineIndex < lineIndex1;)
    {
        TextItem *item = static_cast<TextItem *>(itemAt(lineIndex));
        int currentByteOffset1 = currentByteOffset0 + item->text()->count();
        if (range->i0() <= currentByteOffset0 && currentByteOffset1 <= range->i1() && lines_->count() > 1) {
            lines_->removeAt(lineIndex);
        }
        else {
            item->text = item->text()->copy(range->i0() - currentByteOffset0, range->i1() - currentByteOffset0);
            lines_->updateExtentAt(lineIndex, item->text()->count());
            ++lineIndex;
        }
        currentByteOffset0 = currentByteOffset1;
    }
}

View *TextEdit::addDelegate(View *parent, Item *item)
{
    return Object::create<TextLine>(this, static_cast<TextItem *>(item));
}

void TextEdit::updateDelegateInView(View *delegate, int layoutIndex)
{
    static_cast<TextLine *>(delegate)->lineNumber = layoutIndex;
}

}} // namespace cc::ui
