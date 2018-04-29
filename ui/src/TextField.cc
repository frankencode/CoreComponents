/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/Column>
#include <cc/ui/HLine>
#include <cc/ui/Label>
#include <cc/ui/TextInput>
#include <cc/ui/TextField>

namespace cc {
namespace ui {

TextField::TextField(View *parent, const String &labelText_):
    Control(parent),
    labelText(labelText_)
{
    // color = Color::Red(10);

    organize<Column>();

    Label *label = add<Label>();
    label->text->bind([=]{ return labelText(); });
    label->padding = dp(16);

    input_ = add<TextInput>();
    input_->focus->bind([=]{ return focus(); });
    input_->visible->bind([=]{ return input_->text() != "" || focus(); });
    input_->padding = dp(8);

    label->ink->bind([=]{
        return (pressed() || focus()) ?
            style()->theme()->focusTextColor() :
            style()->theme()->secondaryTextColor();
    });
    label->font->bind([=]{
        return input_->visible() ? app()->smallFont() : app()->defaultFont();
    });

    Label *dummy = add<Label>();
    dummy->visible->bind([=]{ return dummy->text() != "" && !input_->visible(); });
    dummy->text->bind([=]{ return placeholderText(); });
    dummy->padding = dp(8);

    HLine *inputLine = add<HLine>(dp(2));
    inputLine->thickness->bind([=]{ return (hover() || pressed() || focus()) ? dp(2) : dp(1); });
    inputLine->padding->bind([=]{ return dp(8) - inputLine->size()[1]; });
    inputLine->ink->bind([=]{
        if (pressed()) return style()->theme()->pressedInputLineColor();
        if (focus()) return style()->theme()->focusInputLineColor();
        if (hover()) return style()->theme()->hoverInputLineColor();
        return style()->theme()->inputLineColor();
    });

    View *messageArea = add<View>();
    messageArea->size->bind([=]{ return Size { size()[0], sp(12) }; });
    messageArea->padding = dp(8);

    Label *help = messageArea->add<Label>();
    help->font->bind([=]{ return app()->smallFont(); });
    help->ink->bind([=]{ return style()->theme()->secondaryTextColor(); });
    help->text->bind([=]{ return helpText(); });

    Label *error = messageArea->add<Label>();
    error->font->bind([=]{ return app()->smallFont(); });
    error->ink->bind([=]{ return style()->theme()->alertColor(); });
    error->text->bind([=]{ return "Error: " + errorText(); });

    Label *status = messageArea->add<Label>();
    status->font->bind([=]{ return app()->smallFont(); });
    status->text->bind([=]{ return statusText(); });
    status->pos->bind([=]{ return Point { messageArea->size()[0] - status->size()[0], 0 }; });

    help->visible->bind([=]{ return helpText() != "" && !error->visible() && input_->visible(); } );
    status->visible->bind([=]{ return statusText() != "" && !error->visible() && focus(); });
    error->visible->bind([=]{ return errorText() != "" && focus(); });
    messageArea->visible->bind([=]{ return help->visible() || error->visible() || status->visible(); });

    cursor->bind([=]{ return Cursor::create(focus() ? CursorShape::IBeam : CursorShape::Hand); });
}

bool TextField::onPointerClicked(const PointerEvent *event)
{
    if (!focus())
        acquireTextInputFocus();
    return true;
}

}} // namespace cc::ui
