/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/ColumnLayout>
#include <cc/ui/HLine>
#include <cc/ui/TextLine>
#include <cc/ui/TextRun>
#include <cc/ui/TextField>

namespace cc {
namespace ui {

TextField *TextField::create(View *parent, String nameText)
{
    return Object::create<TextField>(parent, nameText);
}

TextField::TextField(View *parent, String labelText_):
    Control(parent),
    labelText(labelText_)
{
    ColumnLayout::setup(this);

    TextLine *labelTextLine = TextLine::create(this);
    labelTextLine->text->bind([=]{ return labelText(); });
    labelTextLine->font->bind([=]{
        return (inputText() != "" || focus()) ? app()->smallFont() : app()->defaultFont();
    });
    labelTextLine->ink->bind([=]{
        return (pressed() || focus()) ?
            style()->theme()->focusTextColor() :
            style()->theme()->secondaryTextColor();
    });
    labelTextLine->padding = dp(16);

    TextLine *inputTextLine = TextLine::create(this);
    inputTextLine->visible->bind([=]{ return focus(); });
    inputTextLine->text->bind([=]{ return inputText(); });
    inputTextLine->padding = dp(8);

    TextLine *placeholderTextLine = TextLine::create(this);
    placeholderTextLine->visible->bind([=]{ return inputText() == "" && focus(); });
    placeholderTextLine->text->bind([=]{ return placeholderText(); });
    placeholderTextLine->padding = dp(8);

    HLine *inputLine = HLine::create(this, dp(2));
    inputLine->thickness->bind([=]{ return (hover() || pressed() || focus()) ? dp(2) : dp(1); });
    inputLine->padding->bind([=]{ return dp(8) - inputLine->size()[1]; });
    inputLine->ink->bind([=]{
        if (pressed()) return style()->theme()->pressedInputLineColor();
        if (focus()) return style()->theme()->focusInputLineColor();
        if (hover()) return style()->theme()->hoverInputLineColor();
        return style()->theme()->inputLineColor();
    });

    View *messageArea = View::create(this);
    messageArea->size->bind([=]{ return Size { size()[0], sp(12) }; });
    messageArea->padding = dp(8);

    TextLine *helpTextLine = TextLine::create(messageArea);
    helpTextLine->font->bind([=]{ return app()->smallFont(); });
    helpTextLine->ink->bind([=]{ return style()->theme()->secondaryTextColor(); });
    helpTextLine->text->bind([=]{ return helpText(); });

    TextLine *errorTextLine = TextLine::create(messageArea);
    errorTextLine->font->bind([=]{ return app()->smallFont(); });
    helpTextLine->ink->bind([=]{ return style()->theme()->alertColor(); });
    errorTextLine->text->bind([=]{ return "Error: " + errorText(); });

    TextLine *statusTextLine = TextLine::create(this);
    statusTextLine->font->bind([=]{ return app()->smallFont(); });
    statusTextLine->text->bind([=]{ return statusText(); });
    statusTextLine->pos->bind([=]{ return Point { messageArea->size()[0] - statusTextLine->size()[0], 0 }; });

    helpTextLine->visible->bind([=]{ return helpText() != "" && !errorTextLine->visible() && focus(); } );
    statusTextLine->visible->bind([=]{ return statusText() != "" && !errorTextLine->visible() && focus(); });
    errorTextLine->visible->bind([=]{ return errorText() != "" && focus(); });
    messageArea->visible->bind([=]{ return helpTextLine->visible() || errorTextLine->visible() || statusTextLine->visible(); });

    cursor = Cursor::create(CursorShape::IBeam);
}

bool TextField::onPointerClicked(const PointerEvent *event)
{
    if (!focus()) acquireTextInputFocus();
    return true;
}
}} // namespace cc::ui
