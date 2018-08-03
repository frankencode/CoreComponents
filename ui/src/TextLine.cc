/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/TextRun>
#include <cc/ui/TextItem>
#include <cc/ui/TextEdit>
#include <cc/ui/TextLine>

namespace cc {
namespace ui {

TextLine::TextLine(View *parent, TextEdit *textEdit, TextItem *textItem):
    View{parent},
    textEdit_{textEdit},
    textItem_{textItem}
{
    inheritPaper();

    lineNumberRun->bind([=]{
        return TextRun::create(str(lineNumber()), textEdit_->lineNumberFont());
    });

    textRun->bind([=]{
        return
            TextRun::create(textItem->text(), textEdit_->font())
                ->wrap(textEdit_->textWidthSansMargin());
    });

    size->bind([=]{
        const double h1 = lineNumberRun()->size()[1] + 2 * textEdit_->lineNumberMargin()[1];
        const double h2 = textRun()->size()[1] + 2 * textEdit_->textMargin()[1];
        return Size {
            lineNumberRun()->size()[0] + 2 * textEdit_->lineNumberMargin()[0] +
            textRun()->size()[0] + 2 * textEdit_->textMargin()[0],
            h1 < h2 ? h2 : h1
        };
    });
}

void TextLine::paint()
{
    Painter p(this);

    double x = 0;
    if (textEdit_->showLineNumbers()) {
        x += textEdit_->lineNumberWidth();
        p->rectangle(
            Point{0, 0},
            Size{x, size()[1]}
        );
        p->setSource(textEdit_->lineNumberPaper());
        p->fill();

        p->setSource(textEdit_->lineNumberInk());
        p->showTextRun(textEdit_->lineNumberPos(), lineNumberRun());
    }

    p->setSource(textEdit_->ink());
    p->showTextRun(Point{x, 0} + textEdit_->textPos(), textRun());
}

}} // namespace cc::ui
