/*
 * Copyright (C) 2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/PlatformManager>
#include <cc/ui/DisplayManager>
#include <cc/ui/TimeMaster>
#include <cc/ui/FingerEvent>
#include <cc/ui/View>
#include <cc/ui/Application>

namespace cc {
namespace ui {

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

Application::Application()
{}

Application::~Application()
{}

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


void Application::updateCursorOnPointerMoved(Window *window, const PointerEvent *event)
{
    if (cursor() && !window->cursorOwner_->containsGlobal(event->pos())) {
        unsetCursor();
        window->cursorOwner_ = nullptr;
    }
}

bool Application::feedFingerEvent(Window *window, FingerEvent *event)
{
    if (event->action() == PointerAction::Moved)
        updateCursorOnPointerMoved(window, event);

    hoveredView = Ref<View>{};

    return window->feedFingerEvent(event);
}

bool Application::feedMouseEvent(Window *window, MouseEvent *event)
{
    if (event->action() == PointerAction::Moved)
    {
        updateCursorOnPointerMoved(window, event);

        if (hoveredView() && !hoveredView()->containsGlobal(event->pos()))
            hoveredView = Ref<View>{};
    }
    else if (event->action() == PointerAction::Pressed)
    {
        hoveredView = Ref<View>{};
    }
    else if (event->action() == PointerAction::Released)
    {
        if (hoveredView() && !hoveredView()->containsGlobal(event->pos()))
            hoveredView = Ref<View>{};
    }

    return window->feedMouseEvent(event);
}

bool Application::feedWheelEvent(Window *window, WheelEvent *event)
{
    return window->feedWheelEvent(event);
}

bool Application::feedKeyEvent(Window *window, KeyEvent *event)
{
    return window->feedKeyEvent(event);
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
