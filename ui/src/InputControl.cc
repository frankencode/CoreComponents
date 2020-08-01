/*
 * Copyright (C) 2018-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/InputControl>
#include <cc/ui/Application>

namespace cc {
namespace ui {

InputControl::Instance::Instance()
{
    keyPressed >>[=](const KeyEvent *event)
    {
        bool cosumed = false;

        if (
            event->scanCode() == ScanCode::Key_Return ||
            event->scanCode() == ScanCode::Key_Return2
        ) {
            accepted();
            gotoNext();
            cosumed = true;
        }
        else if (
            event->scanCode() == ScanCode::Key_Escape
        ) {
            Application{}->focusControl = Control{nullptr};
            rejected();
            cosumed = true;
        }
        else if (
            event->scanCode() == ScanCode::Key_Tab &&
            !+(event->modifiers() & KeyModifier::Shift) &&
            !+(event->modifiers() & KeyModifier::Alt) &&
            !+(event->modifiers() & KeyModifier::Control)
        ) {
            accepted();
            gotoNext();
            cosumed = true;
        }
        else if (
            event->scanCode() == ScanCode::Key_Tab &&
            +(event->modifiers() & KeyModifier::Shift) &&
            !+(event->modifiers() & KeyModifier::Alt) &&
            !+(event->modifiers() & KeyModifier::Control)
        ) {
            accepted();
            gotoPrevious();
            cosumed = true;
        }

        return cosumed;
    };
}

}} // namespace cc::ui
