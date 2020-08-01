/*
 * Copyright (C) 2017-2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/Window>
#include <cc/ui/Application>
#include <cc/ui/Control>
#include <cc/ui/StyleManager>
#include <cc/QueueInstance>

namespace cc {
namespace ui {

Window *Window::open(const View &view, const String &title, WindowMode mode)
{
    return Application{}->openWindow(view, title, mode);
}

Window::Window(const View &view, const String &title):
    title{title},
    size{alias(view->size)},
    view_{view},
    nextFrame_{Frame::create()}
{
    view_->window_ = this;
    if (!view_->paper()) view_->paper <<[=]{ return StyleManager::instance()->activePlugin()->theme()->windowColor(); };
    view_->build();
}

Window::~Window()
{
    view_->disband();
}

Control Window::getControlAt(Point pos) const
{
    if (view_) {
        Control control = view_->getControlAt(pos);
        if (!control && view_->containsLocal(pos))
            control = view_->as<Control>();
        return control;
    }

    return Control{nullptr};
}

void Window::addToFrame(const UpdateRequest *request)
{
    if (nextFrame_->count() > 0) {
        if (nextFrame_->back()->equals(request))
            return;
    }
    nextFrame_->pushBack(request);
}

void Window::commitFrame()
{
    if (nextFrame_->count() == 0) return;
    renderFrame(nextFrame_);
    nextFrame_ = Frame::create();
}

}} // namespace cc::ui
