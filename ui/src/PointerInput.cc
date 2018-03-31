/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/View>
#include <cc/ui/Window>
#include <cc/ui/TouchEvent>
#include <cc/ui/MouseEvent>
#include <cc/ui/PointerInput>

namespace cc {
namespace ui {

PointerInput::PointerInput(View *view):
    view_(view)
{
    view->pointerInput_ = this;
}

void PointerInput::feed(const TouchEvent *event)
{
    mousePos = view_->mapToLocal(event->pos());

    if (event->action() == PointerAction::Pressed) {
        mouseButton = MouseButton::Left;
        view_->window()->touchTargets_->establish(event->fingerId(), view_);
        pressed();
    }
    else if (event->action() == PointerAction::Released) {
        mouseButton = MouseButton::None;
        released();
        if (view_->containsGlobal(event->pos()))
            clicked();
        view_->window()->touchTargets_->remove(event->fingerId());
    }
    else if (event->action() == PointerAction::Moved) {
        hovered();
    }
}

void PointerInput::feed(const MouseEvent *event)
{
    mouseButton = event->button();
    mousePos = view_->mapToLocal(event->pos());

    if (event->action() == PointerAction::Pressed) {
        view_->window()->pointerTarget_ = view_;
        pressed();
    }
    else if (event->action() == PointerAction::Released) {
        released();
        if (view_->containsGlobal(event->pos()))
            clicked();
        view_->window()->pointerTarget_ = 0;
    }
    else if (event->action() == PointerAction::Moved) {
        hovered();
    }
}

}} // namespace cc::ui
