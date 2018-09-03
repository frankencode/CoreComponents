/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Pile>
#include <cc/ui/TextItem>
#include <cc/ui/TextDocument>

namespace cc {
namespace ui {

TextDocument::TextDocument(const String &initialText)
{
    load(initialText);
}

void TextDocument::load(const String &text)
{
    lines_ = Lines::create();
    // rootItem_ = Item::create();
    if (rootItem_)
        rootItem_->paste(0, rootItem_->count());
    else
        rootItem_ = Item::create();

    String filteredText = filterChunk(text);
    const int n = filteredText->count();
    for (int i = 0; i < n;) {
        int i0 = i;
        i = filteredText->find('\n', i);
        i += (i < n);
        String fragment = filteredText->copy(i0, i);
        lines_->insertAt(
            lines_->count(),
            rootItem_->add<TextItem>(fragment),
            fragment->count()
        );
    }

    if (n == 0)
        lines_->insertAt(0, rootItem_->add<TextItem>(), 0);

    byteCount_ = n;
    lineCount_ = lines_->count();
}

String TextDocument::copy(Range range) const
{
    int lineIndex0 = 0;
    int lineIndex1 = 0;
    int currentByteOffset0 = 0;
    lines_->getView(range->i0(), range->i1(), &lineIndex0, &lineIndex1, &currentByteOffset0);

    Ref<StringList> parts = StringList::create();

    for (int lineIndex = lineIndex0; lineIndex < lineIndex1; ++lineIndex) {
        const TextItem *item = static_cast<const TextItem *>(rootItem_->at(lineIndex));
        parts << item->text()->copy(range->i0() - currentByteOffset0, range->i1() - currentByteOffset0);
        currentByteOffset0 += item->text()->count();
    }

    return parts->join();
}

Range TextDocument::paste(Range range, const String &newChunk)
{
    return TextEditorWithHistory::paste(range, newChunk);
}

String TextDocument::filterChunk(const String &newChunk) const
{
    String filteredChunk(newChunk->count(), '\0');
    int j = 0;
    for (int i = 0; i < newChunk->count(); ++i) {
        uint8_t ch = newChunk->byteAt(i);
        if (ch >= 0x20 || ch == '\n') {
            mutate(filteredChunk)->byteAt(j) = ch;
            ++j;
        }
    }
    mutate(filteredChunk)->truncate(j);
    return filteredChunk;
}

void TextDocument::pasteChunk(Range range, const String &newChunk)
{
    OnScopeExit updateCounts{[=]{
        byteCount_ = lines_->extent();
        lineCount_ = rootItem_->totalCount();
    }};

    cut(range);

    if (newChunk->count() == 0) return;

    Ref<StringList> fragments = filterChunk(newChunk)->split('\n');
    int targetOffset = range->i0();
    for (int fragmentIndex = 0; fragmentIndex < fragments->count(); ++fragmentIndex) {
        pasteFragment(targetOffset, fragments->at(fragmentIndex));
        targetOffset += fragments->at(fragmentIndex)->count();
        if (fragmentIndex != fragments->count() - 1) {
            breakLine(targetOffset);
            ++targetOffset;
        }
    }
}

void TextDocument::cut(Range range)
{
    int lineIndex0 = 0;
    int lineIndex1 = 0;
    int currentByteOffset0 = 0;
    lines_->getView(range->begin(), range->end(), &lineIndex0, &lineIndex1, &currentByteOffset0);

    for (int lineIndex = lineIndex0; lineIndex < lineIndex1;)
    {
        TextItem *item = static_cast<TextItem *>(rootItem_->at(lineIndex));
        int currentByteOffset1 = currentByteOffset0 + item->text()->count();
        if (range->i0() <= currentByteOffset0 && currentByteOffset1 <= range->i1() && lines_->count() > 1) {
            lines_->removeAt(lineIndex);
            rootItem_->removeAt(lineIndex);
        }
        else {
            item->text = item->text()->paste(range->i0() - currentByteOffset0, range->i1() - currentByteOffset0, "");
            lines_->updateExtentAt(lineIndex, item->text()->count());
            ++lineIndex;
        }
        currentByteOffset0 = currentByteOffset1;
    }
}

void TextDocument::pasteFragment(int targetOffset, const String &fragment)
{
    TextItem *line = nullptr;
    int lineIndex = 0;
    int lineOffset = 0;

    if (targetOffset == lines_->extent()) {
        lineIndex = lines_->count() - 1;
        line = lines_->at(lineIndex);
        lineOffset = lines_->getPosAt(lineIndex);
    }
    else if (!lines_->lookup(targetOffset, &line, &lineIndex, &lineOffset))
        return;

    targetOffset -= lineOffset;
    line->text = line->text()->paste(targetOffset, targetOffset, fragment);
    lines_->updateExtentAt(lineIndex, line->text()->count());
}

void TextDocument::breakLine(int targetOffset)
{
    TextItem *line = nullptr;
    int lineIndex = 0;
    int lineOffset = 0;

    if (targetOffset == lines_->extent()) {
        lineIndex = lines_->count() - 1;
        line = lines_->at(lineIndex);
        lineOffset = lines_->getPosAt(lineIndex);
    }
    else if (!lines_->lookup(targetOffset, &line, &lineIndex, &lineOffset))
        return;

    auto newLine = Object::create<TextItem>(
        line->text()->copy(
            targetOffset - lineOffset,
            line->text()->count()
        )
    );

    line->text = line->text()->paste(
        targetOffset - lineOffset,
        line->text()->count(),
        "\n"
    );

    lines_->updateExtentAt(lineIndex, line->text()->count());
    ++lineIndex;
    lines_->insertAt(lineIndex, newLine, newLine->text()->count());
    rootItem_->insertAt(lineIndex, newLine);
}

}} // namespace cc::ui
