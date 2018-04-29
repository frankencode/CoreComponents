/*
 * Copyright (C) 2017-2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/debug> // DEBUG
#include <cc/List>
#include <cc/ui/StyleManager>
#include <cc/ui/Window>
#include <cc/ui/Image>
#include <cc/ui/Control>
#include <cc/ui/View>

namespace cc {
namespace ui {

Ref<View> View::create(View *parent)
{
    return Object::create<View>(parent);
}

View::View(View *parent):
    serial_(0),
    window_(0),
    parent_(0),
    children_(Children::create())
{
    if (parent)
    {
        parent->insertChild(this);

        pos->connect([=]{
            if (visible()) update(UpdateReason::Moved);
        });
    }
    else {
        pos->restrict([](Point&, Point) { return false; });
    }

    size ->connect([=]{ update(UpdateReason::Resized); });
    color->connect([=]{ update(UpdateReason::Changed); });
    angle->connect([=]{ update(UpdateReason::Moved); });
    scale->connect([=]{ update(UpdateReason::Moved); });

    visible->connect([=]{
        update(visible() ? UpdateReason::Shown : UpdateReason::Hidden);
    });
}

View::~View()
{
    layout_ = 0;
        // destroy the layout before releasing the children for efficiency
}

void View::disband()
{
    CC_ASSERT2(parent_, "Cannot manually destroy the top-level view");
    if (!parent_) return;
    visible = false;
    parent_->removeChild(this);
}

Point View::mapToGlobal(Point l) const
{
    for (const View *view = this; view->parent_; view = view->parent_)
        l += view->pos();
    return l;
}

Point View::mapToLocal(Point g) const
{
    for (const View *view = this; view->parent_; view = view->parent_)
       g -= view->pos();
    return g;
}

bool View::containsLocal(Point l) const
{
    return
        0 <= l[1] && l[1] < size()[1] &&
        0 <= l[0] && l[0] < size()[0];
}

bool View::containsGlobal(Point g) const
{
    return containsLocal(mapToLocal(g));
}

View *View::getTopViewAt(Point g)
{
    for (auto pair: children_)
    {
        View *child = pair->value();
        if (child->containsGlobal(g)) {
            View *grandChild = child->getTopViewAt(g);
            return (child == grandChild) ? child : grandChild;
        }
    }

    return this;
}

Control *View::getTopControlAt(Point g)
{
    View *view = getTopViewAt(g);
    while (view && !Object::cast<Control *>(view)) view = view->parent();
    return view ? Object::cast<Control *>(view) : nullptr;
}

void View::inheritColor()
{
    for (View *view = parent(); view; view = view->parent()) {
        if (view->color()) {
            color->bind([=]{ return view->color(); });
            return;
        }
    }

    color->bind([=]{ return style()->theme()->windowColor(); });
}

void View::centerInParent()
{
    if (parent())
        pos->bind([=]{ return 0.5 * (parent()->size() - size()); });
}

bool View::isOpaque() const
{
    return color()->isOpaque();
}

bool View::isPainted() const
{
    return color()->isValid();
}

bool View::isStatic() const
{
    return true;
}

void View::clear()
{
    image()->clear(color()->premultiplied());
}

void View::paint()
{}

bool View::onPointerPressed(const PointerEvent *event) { return false; }
bool View::onPointerReleased(const PointerEvent *event) { return false; }
bool View::onPointerClicked(const PointerEvent *event) { return false; }
bool View::onPointerMoved(const PointerEvent *event) { return false; }

bool View::onMousePressed(const MouseEvent *event) { return false; }
bool View::onMouseReleased(const MouseEvent *event) { return false; }
bool View::onMouseClicked(const MouseEvent *event) { return false; }
bool View::onMouseMoved(const MouseEvent *event) { return false; }

bool View::onFingerPressed(const FingerEvent *event) { return false; }
bool View::onFingerReleased(const FingerEvent *event) { return false; }
bool View::onFingerClicked(const FingerEvent *event) { return false; }
bool View::onFingerMoved(const FingerEvent *event) { return false; }

bool View::onWheelMoved(const WheelEvent *event) { return false; }

bool View::onKeyPressed(const KeyEvent *event) { return false; }
bool View::onKeyReleased(const KeyEvent *event) { return false; }

void View::update(UpdateReason reason)
{
    Window *w = window();
    if (!w) return;

    if (
        isPainted() &&
        (reason == UpdateReason::Changed || reason == UpdateReason::Resized)
    ) {
        clear();
        paint();
    }

    if (!visible() && reason != UpdateReason::Hidden) return;

    w->addToFrame(UpdateRequest::create(reason, this));
}

bool View::feedFingerEvent(FingerEvent *event)
{
    PointerEvent::PosGuard guard(&event->pos_, mapToLocal(event->pos()));

    if (event->action() == PointerAction::Pressed)
    {
        if (onPointerPressed(event) || onFingerPressed(event))
            return true;
    }
    else if (event->action() == PointerAction::Released)
    {
        bool eaten = onPointerReleased(event) || onFingerReleased(event);

        if (containsLocal(event->pos()))
        {
            if (onPointerClicked(event) || onFingerClicked(event))
                eaten = true;
        }

        if (eaten) return true;
    }
    else if (event->action() == PointerAction::Moved)
    {
        if (onPointerMoved(event) || onFingerMoved(event))
            return true;
    }

    for (auto pair: children_)
    {
        View *child = pair->value();

        if (child->containsGlobal(event->pos()))
        {
            if (child->feedFingerEvent(event))
                return true;
        }
    }

    return false;
}

bool View::feedMouseEvent(MouseEvent *event)
{
    PointerEvent::PosGuard guard(&event->pos_, mapToLocal(event->pos()));

    if (event->action() == PointerAction::Pressed)
    {
        if (onPointerPressed(event) || onMousePressed(event))
            return true;
    }
    else if (event->action() == PointerAction::Released)
    {
        bool eaten = onPointerReleased(event) || onMouseReleased(event);

        if (containsLocal(event->pos()))
        {
            if (onPointerClicked(event) || onMouseClicked(event))
                eaten = true;
        }

        if (eaten) return true;
    }
    else if (event->action() == PointerAction::Moved)
    {
        if (
            (event->button() != MouseButton::None && onPointerMoved(event)) ||
            onMouseMoved(event)
        )
            return true;
    }

    for (auto pair: children_)
    {
        View *child = pair->value();

        if (child->containsGlobal(event->pos()))
        {
            if (child->feedMouseEvent(event))
                return true;
        }
    }

    return false;
}

bool View::feedWheelEvent(WheelEvent *event)
{
    if (containsGlobal(event->mousePos()))
    {
        if (onWheelMoved(event))
            return true;
    }

    for (auto pair: children_)
    {
        View *child = pair->value();

        if (child->feedWheelEvent(event))
            return true;
    }

    return false;
}

bool View::feedKeyEvent(KeyEvent *event)
{
    if (event->action() == KeyAction::Pressed)
    {
        if (onKeyPressed(event)) return true;
    }
    else if (event->action() == KeyAction::Released)
    {
        if (onKeyReleased(event)) return true;
    }

    for (auto pair: children_)
    {
        if (pair->value()->feedKeyEvent(event))
            return true;
    }

    return false;
}

void View::init()
{
    if (parent()) parent()->childReady(this);
}

void View::childReady(View *child)
{
    if (layout_) layout_->childReady(child);
}

void View::childDone(View *child)
{
    if (layout_) layout_->childDone(child);
}

Application *View::app() const
{
    return Application::instance();
}

StylePlugin *View::style() const
{
    return StyleManager::instance()->activePlugin();
}

Window *View::window() const
{
    if (!window_) {
        if (parent_)
            return parent_->window();
    }
    return window_;
}

Image *View::image()
{
    if (!image_ || image_->size() != Size{::ceil(size()[0]), ::ceil(size()[1])})
        image_ = Image::create(size());
    return image_;
}

uint64_t View::nextSerial() const
{
    return (children_->count() > 0) ? children_->keyAt(children_->count() - 1) + 1 : 1;
}

void View::insertChild(View *child)
{
    child->parent_ = this;
    child->serial_ = nextSerial();
    children_->insert(child->serial_, child);
    childCount += 1;
}

void View::removeChild(View *child)
{
    Ref<View> hook = child;
    children_->remove(child->serial_);
    child->serial_ = 0;
    childCount -= 1;
    childDone(child);
}

void View::adoptChild(View *parent, View *child)
{
    parent->insertChild(child);
}

void View::polish(Window *window)
{
    for (int i = 0; i < children_->count(); ++i)
        children_->valueAt(i)->polish(window);

    clear();
    paint();

    window->addToFrame(UpdateRequest::create(UpdateReason::Changed, this));
}

cairo_surface_t *View::cairoSurface() const
{
    return const_cast<View *>(this)->image()->cairoSurface();
}

}} // namespace cc::ui
