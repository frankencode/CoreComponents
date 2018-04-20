/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/Control>

namespace cc {
namespace ui {

Control::Control(View *parent):
    View(parent)
{
    focus->bind([=]{
        return app()->textInputFocus() ? (app()->textInputFocus()->target() == this) : false;
    });

    hover->bind([=]{ return app()->hoveredView() == this; });
}

bool Control::gainTextInputFocus()
{
    return TextInputFocus::create(this)->isValid();
}

void Control::updateCursorOnPointerMoved(const PointerEvent *event) const
{
    if (cursor() && app()->cursor() != cursor()) {
        app()->setCursor(cursor());
        window()->cursorOwner_ = this;
    }
}

bool Control::feedFingerEvent(FingerEvent *event)
{
    if (event->action() == PointerAction::Moved)
    {
        updateCursorOnPointerMoved(event);

        if (cursor() && app()->cursor() != cursor())
            app()->setCursor(cursor());
    }
    else if (event->action() == PointerAction::Pressed)
    {
        pressed = true;
    }
    else if (event->action() == PointerAction::Released)
    {
        pressed = false;
    }

    return View::feedFingerEvent(event);
}

bool Control::feedMouseEvent(MouseEvent *event)
{
    if (event->action() == PointerAction::Moved)
    {
        updateCursorOnPointerMoved(event);

        if (event->button() == MouseButton::None)
            app()->hoveredView = this;
    }
    else if (event->action() == PointerAction::Pressed)
    {
        pressed = true;
    }
    else if (event->action() == PointerAction::Released)
    {
        pressed = false;

        if (containsGlobal(event->pos()))
            app()->hoveredView = this;
    }

    return View::feedMouseEvent(event);
}

}} // namespace cc::ui
