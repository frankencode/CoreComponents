/*
 * Copyright (C) 2017-2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/ApplicationInstance>
#include <cc/ui/PlatformManager>
#include <cc/ui/DisplayManager>
#include <cc/ui/TimeMaster>
#include <cc/ui/FingerEvent>
#include <cc/ui/Control>

namespace cc {
namespace ui {

bool ApplicationInstance::fin_ { false };

ApplicationInstance *ApplicationInstance::instance()
{
    return PlatformManager::instance()->activePlugin()->application();
}

ApplicationInstance::ApplicationInstance()
{
    textInputArea->connect([=]{
        if (focusControl())
            setTextInputArea(textInputArea());
    });

    focusControl->connect([=]{
        if (focusControlSaved_) stopTextInput();
        focusControlSaved_ = focusControl();

        if (focusControl()) {
            startTextInput(focusControl()->window());

            textInputArea->bind([=]{
                if (!focusControl()) return Rect{};
                Rect a = focusControl()->textInputArea();
                return Rect { focusControl()->mapToGlobal(a->pos()), a->size() };
            });

            setTextInputArea(textInputArea());
        }
        else
            textInputArea = Rect{};
    });

    cursorControl->connect([=]{
        if (cursorControl()) {
            cursor->bind([=]{
                return cursorControl() ? cursorControl()->cursor() : nullptr;
            });
        }
        else cursor = nullptr;
    });

    cursor->connect([=]{
        if (cursor()) setCursor(cursor());
        else unsetCursor();
    });
}

ApplicationInstance::~ApplicationInstance()
{
    fin_ = true;
}

FontSmoothing ApplicationInstance::fontSmoothing() const
{
    if (fontSmoothing_ == FontSmoothing::Default)
        return PlatformManager::instance()->activePlugin()->displayManager()->defaultFontSmoothing();

    return fontSmoothing_;
}

bool ApplicationInstance::pointerIsDragged(const PointerEvent *event, Point dragStart) const
{
    double minDragDistance = Object::cast<const MouseEvent *>(event) ? minMouseDragDistance() : minTouchDragDistance();
    return absPow2(event->pos() - dragStart) >= minDragDistance * minDragDistance;
}

void ApplicationInstance::notifyTimer(Timer::Instance *t)
{
    if (t->interval_ > 0)
        TimeMaster::instance()->ack();
    t->timeout->emit();
}

bool ApplicationInstance::feedFingerEvent(Window *window, FingerEvent *event)
{
    cursorControl = Control{nullptr};
    hoverControl = Control{nullptr};

    if (event->action() == PointerAction::Pressed) {
        Point pos = window->size() * event->pos();
        Control topControl = window->getControlAt(window->view()->mapToLocal(pos));
        if (topControl) {
            touchTargets_->establish(event->fingerId(), topControl);
            pressedControl = topControl;
        }
    }

    if (event->action() == PointerAction::Released) {
        if (focusControl() != pressedControl())
            focusControl = Control{nullptr};
    }

    View touchTarget;
    if (touchTargets_->lookup(event->fingerId(), &touchTarget)) {
        if (event->action() == PointerAction::Released)
            touchTargets_->remove(event->fingerId());
    }

    if (pressedControl()) {
        Point pos = window->size() * event->pos();
        pressedControl()->pointerPos = pressedControl()->mapToLocal(pos);
        bool eaten = pressedControl()->feedFingerEvent(event);
        if (event->action() == PointerAction::Released)
            pressedControl = Control{nullptr};
        if (eaten) return true;
    }
    else if (touchTarget) {
        if (touchTarget->feedFingerEvent(event))
            return true;
    }

    return window->view()->feedFingerEvent(event);
}

bool ApplicationInstance::feedMouseEvent(Window *window, MouseEvent *event)
{
    Control topControl = window->getControlAt(window->view()->mapToLocal(event->pos()));

    if (topControl)
        topControl->pointerPos = topControl->mapToLocal(event->pos());

    if (event->action() == PointerAction::Moved)
    {
        hoverControl = topControl;
    }
    else if (event->action() == PointerAction::Pressed)
    {
        if (!pressedControl())
            pressedControl = topControl;

        hoverControl = Control{nullptr};
    }
    else if (event->action() == PointerAction::Released)
    {
        if (focusControl() != pressedControl())
            focusControl = Control{nullptr};

        hoverControl = topControl;
    }

    bool eaten = false;

    if (pressedControl())
    {
        eaten = pressedControl()->feedMouseEvent(event);

        if (event->action() == PointerAction::Released)
            pressedControl = Control{nullptr};
    }

    if (!eaten)
        eaten = window->view()->feedMouseEvent(event);

    cursorControl = topControl;

    return eaten;
}

bool ApplicationInstance::feedWheelEvent(Window *window, WheelEvent *event)
{
    return window->view()->feedWheelEvent(event);
}

bool ApplicationInstance::feedKeyEvent(Window *window, KeyEvent *event)
{
    if (focusControl())
        return focusControl()->feedKeyEvent(event);

    return window->view()->feedKeyEvent(event);
}

bool ApplicationInstance::feedExposedEvent(Window *window)
{
    return window->view()->feedExposedEvent();
}

bool ApplicationInstance::feedEnterEvent(Window *window)
{
    return window->view()->feedEnterEvent();
}

bool ApplicationInstance::feedLeaveEvent(Window *window)
{
    return window->view()->feedLeaveEvent();
}

bool ApplicationInstance::feedTextEditingEvent(const String &text, int start, int length)
{
    if (focusControl()) {
        focusControl()->onTextEdited(text, start, length);
        return true;
    }

    return false;
}

bool ApplicationInstance::feedTextInputEvent(const String &text)
{
    if (focusControl()) {
        focusControl()->onTextInput(text);
        return true;
    }

    return false;
}

}} // namespace cc::ui
