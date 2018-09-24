/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/debug> // DEBUG
#include <cc/ui/TextRun>
#include <cc/ui/TextItem>
#include <cc/ui/TextEdit>
#include <cc/ui/TextLineHandle>
#include <cc/ui/TextLine>

namespace cc {
namespace ui {

TextLine::TextLine(View *parent, TextEdit *textEdit, TextItem *textItem):
    Control{parent},
    textEdit_{textEdit},
    textItem_{textItem}
{
    inheritPaper();

    lineNumberRun->bind([=]{
        return TextRun::create(str(lineNumber()), textEdit_->lineNumberFont());
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

bool TextLine::withinBounds(Point l) const
{
    return
        0 <= l[1] && l[1] < size()[1] &&
        0 <= l[0] && l[0] < parent()->size()[0];
}

bool TextLine::onPointerPressed(const PointerEvent *event)
{
    CC_INSPECT(event->pos());
    auto cursor = wrappedTextRun()->getNearestTextCursorInLine(event->pos() - textEdit_->textPos());
    CC_INSPECT(cursor);
    return true;
}

void TextLine::paint()
{
    Painter p(this);
    p->setSource(textEdit_->ink());
    p->showTextRun(textEdit_->textPos(), wrappedTextRun());
}

}} // namespace cc::ui
