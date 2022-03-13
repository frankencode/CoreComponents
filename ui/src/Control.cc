/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/Control>
#include <cc/ui/Application>

namespace cc::ui {

Control::Control():
    View{createState}
{}

Control &Control::focus(bool newValue)
{
    if (focus() != newValue)
    {
        if (delegate()) {
            delegate().focus(newValue);
        }
        else {
            if (newValue) me().grabFocus();
            else me().releaseFocus();
        }
    }

    return *this;
}

Control Control::delegate() const
{
    return me().delegate();
}

Control::State::State()
{
    hover([this]{ return isParentOf(Application{}.hoverControl()); });
    pressed([this]{ return pressedOverwrite() || isParentOf(Application{}.pressedControl()); });
    focus([this]{ return isParentOf(Application{}.focusControl()) && window() == Application{}.focusWindow(); });
}

void Control::State::grabFocus()
{
    if (delegate()) {
        delegate()->grabFocus();
    }
    else if (Application{}.focusControl() != self()) {
        Application{}.focusControl(self());
    }
}

void Control::State::releaseFocus()
{
    if (delegate()) {
        delegate()->releaseFocus();
    }
    else if (Application{}.focusControl() == self()) {
        Application{}.focusControl(Control{});
    }
}

Control Control::State::delegate() const
{
    return Control{};
}

Rect Control::State::textInputArea() const
{
    return Rect{Point{}, size()};
}

void Control::State::onTextEdited(const String &text, int start, int length)
{}

void Control::State::onTextInput(const String &text)
{}

} // namespace cc::ui
