/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/TextEdit>
#include <cc/ui/Application>
#include <cc/ui/TextDocument>
#include <cc/ui/TextRun>
#include <cc/ui/TextLine>

namespace cc {
namespace ui {

TextEdit::Instance::Instance()
{
    build >>[=]{
        font <<[=]{ return Application{}->defaultFixedFont(); };
        ink <<[=]{ return theme()->primaryTextColor(); };
        paper <<[=]{ return theme()->popupColor(); };
        textWidth <<[=]{ return size()[0] - lineNumberWidth(); };
        textWidthSansMargin <<[=]{
            double w = textWidth() - 2 * textMargin()[0];
            if (w < 0) w = 0;
            return std::ceil(w);
        };

        lineNumberFont <<[=]{ return font(); };
        lineNumberInk <<[=]{ return theme()->secondaryTextColor(); };
        lineNumberPaper <<[=]{ return theme()->windowColor(); };
        lineNumberWidth <<[=]{
            return
                std::ceil(
                    TextRun::create(String::create(lineNumberDigitCount(), '0'), lineNumberFont())->size()[0] +
                    2 * lineNumberMargin()[0]
                );
        };

        lineNumberAscender <<[=]{
            return lineNumberFont()->getMetrics()->ascender();
        };
        textPos <<[=]{
            return textMargin() + Point{ 0, std::ceil(font()->getMetrics()->ascender()) };
        };

        cursorShape <<[=]{
            return (!readonly() && pointerPos()[0] < textWidth()) ? CursorShape::IBeam : CursorShape::Hand;
        };
        cursor <<[=]{ return Cursor::create(cursorShape()); };
    };
}

void TextEdit::Instance::setText(const String &text)
{
    document_->load(text);
}

String TextEdit::Instance::getText() const
{
    return document_->text();
}

Ref<Item> TextEdit::Instance::initItemModel()
{
    document_ = Object::create<TextDocument>();
    lineCount <<[=]{ return document_->lineCount(); };
    return document_->rootItem();
}

View TextEdit::Instance::createDelegate(Item *item)
{
    return TextLine{this, static_cast<TextItem *>(item)};
}

void TextEdit::Instance::updateDelegateInView(View &delegate, int layoutIndex)
{
    delegate->as<TextLine>()->lineNumber = layoutIndex + 1;
}

}} // namespace cc::ui
