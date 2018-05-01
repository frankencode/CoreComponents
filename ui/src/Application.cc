/*
 * Copyright (C) 2017-2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

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
{
    focusControl->connect([=]{
        if (focusControlSaved_) stopTextInput();
        focusControlSaved_ = focusControl();
        if (focusControl()) {
            Rect a = focusControl()->textInputArea();
            startTextInput(
                focusControl()->window(),
                Rect { focusControl()->mapToGlobal(a->pos()), a->size() }
            );
        }
    });

    cursorControl->connect([=]{
        if (cursorControl()) cursor = cursorControl()->cursor();
        else cursor = nullptr;
    });

    cursor->connect([=]{
        if (cursor()) setCursor(cursor());
        else unsetCursor();
    });
}

Application::~Application()
{
    fin_ = true;
}

FontSmoothing Application::fontSmoothing() const
{
    if (fontSmoothing_ == FontSmoothing::Default)
        return PlatformManager::instance()->activePlugin()->displayManager()->defaultFontSmoothing();

    return fontSmoothing_;
}

void Application::notifyTimer(Timer *t)
{
    if (t->interval_ > 0)
        TimeMaster::instance()->ack();
    t->triggered->emit();
}

bool Application::feedFingerEvent(Window *window, FingerEvent *event)
{
    cursorControl = nullptr;
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
    Control *topControl = window->view()->getTopControlAt(event->pos());

    if (event->action() == PointerAction::Moved)
    {
        hoverControl = topControl;
    }
    else if (event->action() == PointerAction::Pressed)
    {
        if (!pressedControl())
            pressedControl = topControl;

        hoverControl = nullptr;
    }
    else if (event->action() == PointerAction::Released)
    {
        if (focusControl() != pressedControl())
            focusControl = nullptr;

        hoverControl = topControl;
    }

    bool eaten = false;

    if (pressedControl())
    {
        eaten = pressedControl()->feedMouseEvent(event);

        if (event->action() == PointerAction::Released)
            pressedControl = nullptr;
    }

    if (!eaten)
        eaten = window->view()->feedMouseEvent(event);

    cursorControl = topControl;

    return eaten;
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
    if (focusControl()) {
        focusControl()->onTextEdited(text, start, length);
        return true;
    }

    return false;
}

bool Application::feedTextInputEvent(const String &text)
{
    if (focusControl()) {
        focusControl()->onTextInput(text);
        return true;
    }

    return false;
}

}} // namespace cc::ui
