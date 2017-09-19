/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/Application>
#include <cc/ui/Window>

namespace cc {
namespace ui {

Ref<Window> Window::open(String title, int width, int height, Color color)
{
    return Application::instance()->openWindow(title, width, height, color);
}

Window::Window(String title):
    windowMoved_(WindowEvent::create()),
    windowResized_(WindowEvent::create()),
    title_(title)
{}

Window::~Window()
{}

}} // namespace cc::ui
