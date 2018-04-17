/*
 * Copyright (C) 2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Queue>
#include <cc/ui/Application>
#include <cc/ui/View>
#include <cc/ui/Window>

namespace cc {
namespace ui {

Window *Window::open(View *view, String title, WindowMode mode)
{
    return Application::instance()->openWindow(view, title, mode);
}

Window::Window(View *view, String title):
    title(title),
    size(alias(view->size)),
    view_(view),
    nextFrame_(Frame::create()),
    touchTargets_(TouchTargets::create())
{
    view_->window_ = this;
    view_->polish(this);
}

Window::~Window()
{}

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

bool Window::feedFingerEvent(FingerEvent *event)
{
    Ref<View> touchTarget;

    if (touchTargets_->lookup(event->fingerId(), &touchTarget))
    {
        if (event->action() == PointerAction::Released)
            touchTargets_->remove(event->fingerId());

        if (touchTarget->feedFingerEvent(event))
            return true;
    }

    return view_->feedFingerEvent(event);
}

bool Window::feedMouseEvent(MouseEvent *event)
{
    if (pointerTarget_)
    {
        bool eaten = pointerTarget_->feedMouseEvent(event);

        if (event->action() == PointerAction::Released)
            pointerTarget_ = 0;

        if (eaten) return true;
    }

    return view_->feedMouseEvent(event);
}

bool Window::feedWheelEvent(WheelEvent *event)
{
    return view_->feedWheelEvent(event);
}

bool Window::feedKeyEvent(KeyEvent *event)
{
    return view_->feedKeyEvent(event);
}

}} // namespace cc::ui
