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
    View{parent}
{
    hover->bind([=]{ return isParentOf(app()->hoverControl()); });
    pressed->bind([=]{ return isParentOf(app()->pressedControl()); });
    focus->bind([=]{ return isParentOf(app()->focusControl()); });
}

bool Control::onPointerClicked(const PointerEvent *event)
{
    return false;
}

bool Control::onMouseClicked(const MouseEvent *event)
{
    return false;
}

bool Control::onFingerClicked(const FingerEvent *event)
{
    return false;
}

Rect Control::textInputArea() const
{
    return Rect{ Point{}, size() };
}

void Control::onTextEdited(const String &text, int start, int length)
{}

void Control::onTextInput(const String &text)
{}

}} // namespace cc::ui
