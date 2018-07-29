/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Pile>
#include <cc/ui/TextModel>

namespace cc {
namespace ui {

TextModel::TextModel():
    text_{Lines::create()}
{
    text_->insertAt(0, add<TextItem>(), 0);
}

String TextModel::copy(Range range) const
{
    int lineIndex0 = 0;
    int lineIndex1 = 0;
    int currentByteOffset0 = 0;
    text_->getView(range->i0(), range->i1(), &lineIndex0, &lineIndex1, &currentByteOffset0);

    Ref<StringList> parts = StringList::create();

    for (int lineIndex = lineIndex0; lineIndex < lineIndex1; ++lineIndex) {
        const TextItem *item = static_cast<const TextItem *>(at(lineIndex));
        parts << item->text()->copy(range->i0() - currentByteOffset0, range->i1() - currentByteOffset0);
        currentByteOffset0 += item->text()->count();
    }

    return parts->join();
}

Range TextModel::paste(Range range, const String &newChunk)
{
    return TextEditorWithHistory::paste(range, newChunk);
}

String TextModel::filterChunk(const String &newChunk) const
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

void TextModel::pasteChunk(Range range, const String &newChunk)
{
    OnScopeExit updateCounts{[=]{
        charCount_ = text_->extent();
        lineCount_ = totalCount();
    }};

    cut(range);

    if (newChunk->count() == 0) return;

    Ref<StringList> fragments = newChunk->split('\n');
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

void TextModel::cut(Range range)
{
    int lineIndex0 = 0;
    int lineIndex1 = 0;
    int currentByteOffset0 = 0;
    text_->getView(range->begin(), range->end(), &lineIndex0, &lineIndex1, &currentByteOffset0);

    for (int lineIndex = lineIndex0; lineIndex < lineIndex1;)
    {
        TextItem *item = static_cast<TextItem *>(at(lineIndex));
        int currentByteOffset1 = currentByteOffset0 + item->text()->count();
        if (range->i0() <= currentByteOffset0 && currentByteOffset1 <= range->i1() && text_->count() > 1) {
            text_->removeAt(lineIndex);
        }
        else {
            item->text = item->text()->paste(range->i0() - currentByteOffset0, range->i1() - currentByteOffset0, "");
            text_->updateExtentAt(lineIndex, item->text()->count());
            ++lineIndex;
        }
        currentByteOffset0 = currentByteOffset1;
    }
}

void TextModel::pasteFragment(int targetOffset, const String &fragment)
{
    TextItem *line = nullptr;
    int lineIndex = 0;
    int lineOffset = 0;

    if (targetOffset == text_->extent()) {
        lineIndex = text_->count() - 1;
        line = text_->at(lineIndex);
        lineOffset = text_->getPosAt(lineIndex);
    }
    else if (!text_->lookup(targetOffset, &line, &lineIndex, &lineOffset))
        return;

    targetOffset -= lineOffset;
    line->text = line->text()->paste(targetOffset, targetOffset, fragment);
    text_->updateExtentAt(lineIndex, line->text()->count());
}

void TextModel::breakLine(int targetOffset)
{
    TextItem *line = nullptr;
    int lineIndex = 0;
    int lineOffset = 0;

    if (targetOffset == text_->extent()) {
        lineIndex = text_->count() - 1;
        line = text_->at(lineIndex);
        lineOffset = text_->getPosAt(lineIndex);
    }
    else if (!text_->lookup(targetOffset, &line, &lineIndex, &lineOffset))
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

    text_->updateExtentAt(lineIndex, line->text()->count());
    ++lineIndex;
    insertAt(lineIndex, newLine);
    text_->insertAt(lineIndex, newLine, newLine->text()->count());
}

}} // namespace cc::ui
