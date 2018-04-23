/*
 * Copyright (C) 2017-2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/debug>
#include <cc/ui/PlatformManager>
#include <cc/ui/DisplayManager>
#include <cc/ui/TimeMaster>
#include <cc/ui/FingerEvent>
#include <cc/ui/Control>
#include <cc/ui/Application>

namespace cc {
namespace ui {

bool Application::fin_ { false };

Application *Application::open(int argc, char **argv)
{
    Application *app = Application::instance();
    app->init(argc, argv);
    return app;
}

Application *Application::instance()
{
    return PlatformManager::instance()->activePlugin()->application();
}

Application::Application():
    touchTargets_(TouchTargets::create())
{}

Application::~Application()
{
    fin_ = true;
}

TextSmoothing Application::textSmoothing() const
{
    if (textSmoothing_ == TextSmoothing::Default)
        return PlatformManager::instance()->activePlugin()->displayManager()->defaultTextSmoothing();

    return textSmoothing_;
}

void Application::notifyTimer(Timer *t)
{
    if (t->interval_ > 0)
        TimeMaster::instance()->ack();
    t->triggered->emit();
}

bool Application::feedFingerEvent(Window *window, FingerEvent *event)
{
    if (cursor()) {
        unsetCursor();
        cursorWindow_->cursorControl_ = nullptr;
        cursorWindow_ = nullptr;
    }

    if (hoverControl())
        hoverControl = nullptr;


    if (event->action() == PointerAction::Pressed)
    {
        Control *control = window->view()->getTopControlAt(event->pos());
        if (control)
            touchTargets_->establish(event->fingerId(), control);
    }

    Ref<View> touchTarget;
    if (touchTargets_->lookup(event->fingerId(), &touchTarget))
    {
        if (event->action() == PointerAction::Released)
            touchTargets_->remove(event->fingerId());

        if (touchTarget->feedFingerEvent(event))
            return true;
    }

    return window->view()->feedFingerEvent(event);
}

bool Application::feedMouseEvent(Window *window, MouseEvent *event)
{
    if (event->action() == PointerAction::Moved)
    {
        Control *control = window->view()->getTopControlAt(event->pos());

        if (control && control->cursor() && cursor() != control->cursor()) {
            setCursor(control->cursor());
            cursorWindow_ = window;
            cursorWindow_->cursorControl_ = control;
        }
        else if (cursor()) {
            if (cursorWindow_ != window || !cursorWindow_->cursorControl_->containsGlobal(event->pos())) {
                unsetCursor();
                cursorWindow_->cursorControl_ = nullptr;
                cursorWindow_ = nullptr;
            }
        }

        hoverControl = control;
    }
    else if (event->action() == PointerAction::Pressed)
    {
        if (!pressedControl()) {
            Control *control = window->view()->getTopControlAt(event->pos());
            pressedControl = control;
        }

        if (
            textInputFocus() && textInputFocus()->isValid() &&
            !(pressedControl() && pressedControl()->focus())
        )
            textInputFocus = nullptr;

        hoverControl = nullptr;
    }
    else if (event->action() == PointerAction::Released)
    {
        hoverControl = window->view()->getTopControlAt(event->pos());
    }

    if (pressedControl())
    {
        bool eaten = pressedControl()->feedMouseEvent(event);

        if (event->action() == PointerAction::Released)
            pressedControl = nullptr;

        if (eaten) return true;
    }

    return window->view()->feedMouseEvent(event);
}

bool Application::feedWheelEvent(Window *window, WheelEvent *event)
{
    return window->view()->feedWheelEvent(event);
}

bool Application::feedKeyEvent(Window *window, KeyEvent *event)
{
    return window->view()->feedKeyEvent(event);
}

bool Application::feedTextEditingEvent(const String &text, int start, int length)
{
    if (textInputFocus() && textInputFocus()->isValid()) {
        textInputFocus()->target()->onTextEdited(text, start, length);
        return true;
    }

    return false;
}

bool Application::feedTextInputEvent(const String &text)
{
    if (textInputFocus() && textInputFocus()->isValid()) {
        textInputFocus()->target()->onTextInput(text);
        return true;
    }

    return false;
}

}} // namespace cc::ui
