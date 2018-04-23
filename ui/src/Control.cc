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
    hover->bind([=]{ return app()->hoverControl() == this; });
    pressed->bind([=]{ return app()->pressedControl() == this; });

    focus->bind([=]{
        return app()->textInputFocus() ? (app()->textInputFocus()->target() == this) : false;
    });
}

Control::~Control()
{
    releaseTextInputFocus();
}

bool Control::acquireTextInputFocus()
{
    if (focus()) return true;
    return TextInputFocus::create(this)->isValid();
}

void Control::releaseTextInputFocus()
{
    if (!Application::fin())
        app()->textInputFocus = nullptr;
}

}} // namespace cc::ui
