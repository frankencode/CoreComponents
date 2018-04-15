/*
 * Copyright (C) 2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/PlatformManager>
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

void Application::notifyTimer(Timer *t)
{
    if (t->interval_ > 0)
        TimeMaster::instance()->ack();
    t->triggered->emit();
}

bool Application::feedFingerEvent(Window *window, FingerEvent *event)
{
    return window->feedFingerEvent(event);
}

bool Application::feedMouseEvent(Window *window, MouseEvent *event)
{
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
