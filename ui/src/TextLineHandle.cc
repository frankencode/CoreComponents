/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/TextLine>
#include <cc/ui/TextEdit>
#include <cc/ui/TextRun>
#include <cc/ui/TextLineHandle>

namespace cc {
namespace ui {

TextLineHandle::TextLineHandle(View *parent, TextLine *textLine):
    View{parent},
    textLine_{textLine}
{
    paper->bind([=]{ return textLine_->textEdit_->lineNumberPaper(); });
    size->bind([=]{ return Size{ textLine_->textEdit_->lineNumberWidth(), textLine_->size()[1] }; });
    pos->bind([=]{ return Point{ textLine_->textEdit_->size()[0] - textLine_->textEdit_->lineNumberWidth(), 0 }; });
}

void TextLineHandle::paint()
{
    Painter p(this);
    if (textLine_->textEdit_->showLineNumbers()) {
        p->setSource(textLine_->textEdit_->lineNumberInk());
        p->showTextRun(textLine_->textEdit_->lineNumberPos(), textLine_->lineNumberRun());
    }
}

}} // namespace cc::ui
