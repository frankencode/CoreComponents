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

void Application::fingerEvent(View *view, FingerEvent *event)
{
    view->fingerEvent(event);
}

void Application::mouseEvent(View *view, MouseEvent *event)
{
    view->mouseEvent(event);
}

void Application::wheelEvent(View *view, WheelEvent *event)
{
    view->wheelEvent(event);
}

void Application::keyEvent(View *view, KeyEvent *event)
{
    view->keyEvent(event);
}

}} // namespace cc::ui
