/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/InputControl>
#include <cc/DEBUG>

namespace cc::ui {

InputControl::State::State()
{
    onKeyPressed(
        [this](const KeyEvent &event)
        {
            bool consumed = false;

            if (
                event.scanCode() == ScanCode::Key_Return ||
                event.scanCode() == ScanCode::Key_Return2
            ) {
                if (accept()) {
                    onAccepted();
                    gotoNext();
                }
                consumed = true;
            }
            else if (
                event.scanCode() == ScanCode::Key_Escape
            ) {
                releaseFocus();
                onRejected();
                consumed = true;
            }
            else if (
                event.scanCode() == ScanCode::Key_Tab &&
                !(event.modifiers() & KeyModifier::Shift) &&
                !(event.modifiers() & KeyModifier::Alt) &&
                !(event.modifiers() & KeyModifier::Control)
            ) {
                if (accept()) {
                    onAccepted();
                    gotoNext();
                }
                consumed = true;
            }
            else if (
                event.scanCode() == ScanCode::Key_Tab &&
                (event.modifiers() & KeyModifier::Shift) &&
                !(event.modifiers() & KeyModifier::Alt) &&
                !(event.modifiers() & KeyModifier::Control)
            ) {
                if (accept()) {
                    onAccepted();
                    gotoPrevious();
                }
                consumed = true;
            }

            return consumed;
        }
    );
}

void InputControl::State::gotoNext()
{
    List<InputControl> inputControls;
    self().root().collectVisible<InputControl>(&inputControls);
    Locator target;
    inputControls.find(self(), &target);
    if (++target) {
        inputControls.mutableAt(target).focus(true);
    }
    /*else {
        CC_DEBUG;
        releaseFocus();
    }*/
}

void InputControl::State::gotoPrevious()
{
    List<InputControl> inputControls;
    self().root().collectVisible<InputControl>(&inputControls);
    Locator target;
    inputControls.find(self(), &target);
    if (--target) {
        inputControls.mutableAt(target).focus(true);
    }
    /*else {
        CC_DEBUG;
        releaseFocus();
    }*/
}

} // namespace cc::ui
