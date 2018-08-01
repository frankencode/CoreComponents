/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/TextDocument>
#include <cc/ui/TextRun>
#include <cc/ui/TextLine>
#include <cc/ui/TextEdit>

namespace cc {
namespace ui {

TextEdit::TextEdit(View *parent):
    ItemView{parent}
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
}

Ref<Item> TextEdit::initItemModel()
{
    document_ = Object::create<TextDocument>();
    lineCount->bind([=]{ return document_->lineCount(); });
    return document_->rootItem();
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
