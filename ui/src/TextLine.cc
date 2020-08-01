/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/TextLine>
#include <cc/ui/TextRun>
#include <cc/ui/TextItem>
#include <cc/ui/TextEdit>
#include <cc/ui/TextLineHandle>

namespace cc {
namespace ui {

TextLine::Instance::Instance(TextEdit::Instance *textEdit, TextItem *textItem):
    textEdit_{textEdit},
    textItem_{textItem}
{
    build >>[=]{
        inheritPaper();

        lineNumberRun <<[=]{
            return TextRun::create(str(lineNumber()), textEdit_->lineNumberFont());
        };

        textRun <<[=]{
            return TextRun::create(textItem->text(), textEdit_->font());
        };

        wrappedTextRun <<[=]{
            return textRun()->wrap(textEdit_->textWidthSansMargin());
        };

        size <<[=]{
            const double h1 = lineNumberRun()->size()[1] + 2 * textEdit_->lineNumberMargin()[1];
            const double h2 = wrappedTextRun()->size()[1] + 2 * textEdit_->textMargin()[1];
            return Size {
                textEdit_->lineNumberWidth() +
                wrappedTextRun()->size()[0] + 2 * textEdit_->textMargin()[0],
                h1 < h2 ? h2 : h1
            };
        };

        (*this) << TextLineHandle{this};
    };

    paint <<[=]{
        Painter p{this};
        p->setSource(textEdit_->ink());
        p->showTextRun(textEdit_->textPos(), wrappedTextRun());
    };

    pointerPressed >>[=](const PointerEvent *event)
    {
        auto cursor = wrappedTextRun()->getNearestTextCursorInLine(event->pos() - textEdit_->textPos());
        // \todo ...
        return true;
    };
}

bool TextLine::Instance::withinBounds(Point l) const
{
    return
        0 <= l[1] && l[1] < size()[1] &&
        0 <= l[0] && l[0] < parent()->size()[0];
}

}} // namespace cc::ui
