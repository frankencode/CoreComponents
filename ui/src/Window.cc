/*
 * Copyright (C) 2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Queue>
#include <cc/ui/Application>
#include <cc/ui/Window>

namespace cc {
namespace ui {

Window *Window::open(View *view, String title, WindowMode mode)
{
    Window *window = Application::instance()->openWindow(view, title, mode);
    view->update(UpdateRequest::create(UpdateReason::Changed, view)); // FIXME: rubbish, implement exposed event handler
    return window;
}

Window::Window(View *view, String title):
    visible(alias(view->visible)),
    title(title),
    pos(alias(view->pos)),
    size(alias(view->size)),
    color(alias(view->color)),
    view_(view),
    nextFrame_(Frame::create()),
    touchTargets_(TouchTargets::create())
{
    view_->window_ = this;
}

Window::~Window()
{}

void Window::addToFrame(const UpdateRequest *request)
{
    nextFrame_->pushBack(request);
}

void Window::commitFrame()
{
    if (nextFrame_->count() == 0) return;
    renderFrame(nextFrame_);
    nextFrame_ = Frame::create();
}

}} // namespace cc::ui
