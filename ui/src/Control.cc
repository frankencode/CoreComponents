/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Control>
#include <cc/Application>

namespace cc {

Control::State::State()
{
    hover([this]{
        if (Application{}.hoverControl() == this) return true;
        for (Control d = delegate(); d; d = d->delegate()) {
            if (Application{}.hoverControl() == d) return true;
        }
        return false;
    });

    focus([this]{
        if (window() != Application{}.focusWindow()) return false;
        if (Application{}.focusControl() == this) return true;
        for (Control d = delegate(); d; d = d->delegate()) {
            if (Application{}.focusControl() == d) return true;
        }
        return false;
    });

    pressed([this]{
        if (pressedOverwrite()) return true;
        if (Application{}.pressedControl() == this) return true;
        for (Control d = delegate(); d; d = d->delegate()) {
            if (Application{}.pressedControl() == d) return true;
        }
        return false;
    });
}

void Control::State::grabFocus()
{
    if (delegate()) {
        delegate()->grabFocus();
    }
    else if (Application{}.focusControl() != self()) {
        if (hasWindow()) {
            Application{}.focusControl(self());
        }
        else {
            Application{}.postEvent([target=self()]{
                Application{}.focusControl(target);
            });
        }
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

} // namespace cc
