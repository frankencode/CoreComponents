/*
 * Copyright (C) 2017-2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Queue>
#include <cc/ui/Application>
#include <cc/ui/Control>
#include <cc/ui/Window>

namespace cc {
namespace ui {

Window *Window::open(View *view, const String &title, WindowMode mode)
{
    return Application::instance()->openWindow(view, title, mode);
}

Window::Window(View *view, const String &title):
    title{title},
    size{alias(view->size)},
    view_{view},
    nextFrame_{Frame::create()}
{
    view_->window_ = this;
    view_->polish(this);
}

Window::~Window()
{}

Control *Window::getControlAt(Point pos) const
{
    if (view_) {
        Control *control = view_->getControlAt(pos);
        if (!control && view_->containsLocal(pos))
            control = Object::cast<Control *>(view_);
        return control;
    }

    return nullptr;
}

Object *Window::getContext(View *view)
{
    return view->context_;
}

void Window::setContext(View *view, Object *context)
{
    view->context_ = context;
}

bool Window::isOpaque(View *view)
{
    return view->isOpaque();
}

bool Window::isPainted(View *view)
{
    return view->isPainted();
}

bool Window::isStatic(View *view)
{
    return view->isStatic();
}

Image *Window::image(View *view)
{
    return view->image();
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
