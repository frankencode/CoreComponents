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
#include <cc/ui/TextField>

namespace cc {
namespace ui {

TextField *TextField::create(View *parent, String nameText)
{
    return Object::create<TextField>(parent, nameText);
}

TextField::TextField(View *parent, String labelText_):
    View(parent),
    labelText(labelText_)
{
    focus->bind([=]{
        return app()->textInputFocus() ? (app()->textInputFocus()->target() == this) : false;
    });

    ColumnLayout::setup(this);

    labelTextLine_ = TextLine::create(this);
    labelTextLine_->text->bind([=]{ return labelText(); });
    labelTextLine_->font->bind([=]{ return Font( (inputText() != "" || focus()) ? sp(12) : sp(16) ) << Color::shade(54); });
    labelTextLine_->padding = dp(16);

    inputTextLine_ = TextLine::create(this, Font(sp(16)));
    inputTextLine_->visible->bind([=]{ return focus(); });
    inputTextLine_->text->bind([=]{ return inputText(); });
    inputTextLine_->padding = dp(8);

    placeholderTextLine_ = TextLine::create(this, Font(sp(16)));
    placeholderTextLine_->visible->bind([=]{ return inputText() == "" && focus(); });
    placeholderTextLine_->text->bind([=]{ return placeholderText(); });
    placeholderTextLine_->padding = dp(8);

    inputLine_ = HLine::create(this, dp(2));
    inputLine_->thickness->bind([=]{ return focus() ? dp(2) : dp(1); });
    inputLine_->padding->bind([=]{ return dp(8) - inputLine_->size()[1]; });
    inputLine_->ink = Color::shade(42);

    View *messageArea = View::create(this);
    messageArea->size->bind([=]{ return Size { size()[0], sp(12) }; });
    messageArea->padding = dp(8);

    helpTextLine_ = TextLine::create(messageArea, Font(sp(12)) << Color::shade(54));
    helpTextLine_->text->bind([=]{ return helpText(); });

    errorTextLine_ = TextLine::create(messageArea, Font(sp(12)));
    errorTextLine_->text->bind([=]{ return "Error: " + errorText(); });

    statusTextLine_ = TextLine::create(this, Font(sp(12)));
    statusTextLine_->text->bind([=]{ return statusText(); });
    statusTextLine_->pos->bind([=]{ return Point { messageArea->size()[0] - statusTextLine_->size()[0], 0 }; });

    helpTextLine_->visible->bind([=]{ return helpText() != "" && !errorTextLine_->visible() && focus(); } );
    statusTextLine_->visible->bind([=]{ return statusText() != "" && !errorTextLine_->visible() && focus(); });
    errorTextLine_->visible->bind([=]{ return errorText() != "" && focus(); });
    messageArea->visible->bind([=]{ return helpTextLine_->visible() || errorTextLine_->visible() || statusTextLine_->visible(); });

    cursor = Cursor::create(CursorShape::IBeam);
}

bool TextField::onPointerPressed(const PointerEvent *event)
{
    pressed = true;
    return true;
}

bool TextField::onPointerReleased(const PointerEvent *event)
{
    pressed = false;
    return true;
}

}} // namespace cc::ui
