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

TextLine::TextLine(TextEdit *textView, TextItem *textItem):
    textView_{textView},
    textItem_{textItem}
{
    lineNumberRun->bind([=]{
        return TextRun::create(str(lineNumber()), textView_->lineNumberFont());
    });

    textRun->bind([=]{
        return
            TextRun::create(textItem->text(), textView_->font())
                ->wrap(textView_->textWidthSansMargin());
    });

    size->bind([=]{
        const double h1 = lineNumberRun()->size()[1] + 2 * textView_->lineNumberMargin()[1];
        const double h2 = textRun()->size()[1] + 2 * textView_->textMargin()[1];
        return Size {
            textView_->size()[0],
            h1 < h2 ? h2 : h1
        };
    });
}

void TextLine::paint()
{
    Painter p(this);

    double x = 0;
    if (textView_->showLineNumbers()) {
        x += textView_->lineNumberWidth();
        p->rectangle(
            Point{0, 0},
            Size{x, size()[1]}
        );
        p->setSource(textView_->lineNumberPaper());
        p->fill();

        p->setSource(textView_->lineNumberInk());
        p->showTextRun(textView_->lineNumberPos(), lineNumberRun());
    }

    p->rectangle(
        Point{x, 0},
        size() - Size{x, 0}
    );
    p->setSource(textView_->paper());
    p->fill();

    p->setSource(textView_->ink());
    p->showTextRun(Point{x, 0} + textView_->textPos(), textRun());
}

}} // namespace cc::ui
