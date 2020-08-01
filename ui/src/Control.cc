/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/Control>
#include <cc/ui/Application>

namespace cc {
namespace ui {

Control::Instance::Instance()
{
    hover <<[=]{ return isParentOf(Application{}->hoverControl()); };
    pressed <<[=]{ return isParentOf(Application{}->pressedControl()); };
    focus <<[=]{ return isParentOf(Application{}->focusControl()); };
}

Rect Control::Instance::textInputArea() const
{
    return Rect{Point{}, size()};
}

void Control::Instance::onTextEdited(const String &text, int start, int length)
{}

void Control::Instance::onTextInput(const String &text)
{}

}} // namespace cc::ui
