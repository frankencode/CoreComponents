/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/View>
#include <cc/ui/KeyEvent>
#include <cc/ui/KeyInput>

namespace cc {
namespace ui {

KeyInput::KeyInput(View *view):
    view_(view)
{
    view_->keyInput_ = this;
}

void KeyInput::feed(const KeyEvent *event)
{
    if (event->action() == KeyAction::Pressed) {
        key = Key{ event->scanCode(), event->keyCode(), event->modifiers() };
        keyPressed();
    }
    else if (event->action() == KeyAction::Released) {
        keyReleased();
        key = Key{};
    }
}

}} // namespaec cc::ui
