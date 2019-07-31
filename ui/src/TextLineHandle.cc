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
        p->showTextRun(
            Point{
                (size()[0] - textLine_->lineNumberRun()->size()[0]) / 2,
                (size()[1] / textLine_->wrappedTextRun()->lineCount() + textLine_->textEdit_->lineNumberAscender()) / 2
            },
            textLine_->lineNumberRun()
        );
    }
    else if ((textLine_->lineNumber() - 1) % 5 == 0) {
        Color c = paper();
        c->mixIn(textLine_->textEdit_->lineNumberInk(), 15);
        p->setSource(c);
        const int n = textLine_->wrappedTextRun()->lineCount();
        const int i = 0;
        //for (int i = 0; i < n; ++i) {
            p->circle(
                Point{ size()[0] / 2, (i + 0.5) * size()[1] / n },
                dp(3.5)
            );
            /*Point center{ size()[0] / 2, (i + 0.5) * size()[1] / n };
            Size size{dp(4)};
            p->rectangle(
                center - size / 2,
                size
            );*/
            p->fill();
            /*p->setSource(paper());
            p->circle(
                Point{ size()[0] / 2, (i + 0.5) * size()[1] / n },
                dp(2)
            );
            p->fill();*/
        //}
    }
}

}} // namespace cc::ui
