/*
 * Copyright (C) 2018-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/InputControl>

namespace cc {
namespace ui {

bool InputControl::onKeyPressed(const KeyEvent *event)
{
    if (
        event->scanCode() == ScanCode::Key_Return ||
        event->scanCode() == ScanCode::Key_Return2
    ) {
        accepted();
        gotoNext();
    }
    else if (
        event->scanCode() == ScanCode::Key_Escape
    ) {
        Application::instance()->focusControl = nullptr;
        rejected();
    }
    else if (
        event->scanCode() == ScanCode::Key_Tab &&
        !+(event->modifiers() & KeyModifier::Shift)
    ) {
        accepted();
        gotoNext();
    }
    else if (
        event->scanCode() == ScanCode::Key_Tab &&
        +(event->modifiers() & KeyModifier::Shift)
    ) {
        accepted();
        gotoPrevious();
    }

    return true;
}

}} // namespace cc::ui
