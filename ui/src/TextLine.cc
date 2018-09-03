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
#include <cc/ui/TextLineHandle>
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
        return TextRun::create(cc::right(str(lineNumber()), textEdit_->lineNumberDigitCount()), textEdit_->lineNumberFont());
    });

    textRun->bind([=]{
        return TextRun::create(textItem->text(), textEdit_->font());
    });

    wrappedTextRun->bind([=]{
        return textRun()->wrap(textEdit_->textWidthSansMargin());
    });

    size->bind([=]{
        const double h1 = lineNumberRun()->size()[1] + 2 * textEdit_->lineNumberMargin()[1];
        const double h2 = wrappedTextRun()->size()[1] + 2 * textEdit_->textMargin()[1];
        return Size {
            textEdit_->lineNumberWidth() +
            wrappedTextRun()->size()[0] + 2 * textEdit_->textMargin()[0],
            h1 < h2 ? h2 : h1
        };
    });

    add<TextLineHandle>(this);
}

void TextLine::paint()
{
    Painter p(this);
    p->setSource(textEdit_->ink());
    p->showTextRun(Point{0, 0} + textEdit_->textPos(), wrappedTextRun());
}

}} // namespace cc::ui
