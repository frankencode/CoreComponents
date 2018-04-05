/*
 * Copyright (C) 2017-2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/debug> // DEBUG
#include <cc/List>
#include <cc/ui/Application>
#include <cc/ui/StyleManager>
#include <cc/ui/Window>
#include <cc/ui/Image>
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
        0 <= l[0] && l[0] < size()[0] &&
        0 <= l[1] && l[1] < size()[1];
}

bool View::containsGlobal(Point g) const
{
    return containsLocal(mapToLocal(g));
}

void View::centerInParent()
{
    if (parent())
        pos->bind([=]{ return 0.5 * (parent()->size() - size()); });
}

bool View::isOpaque() const
{
    return Color::isOpaque(color());
}

bool View::isPainted() const
{
    return Color::isValid(color());
}

bool View::isStatic() const
{
    return true;
}

void View::clear()
{
    image()->clear(Color::premultiplied(color()));
}

void View::paint()
{}

bool View::hasPointerInput() const { return false; }
bool View::onPointerPressed(const PointerEvent *event) { return false; }
bool View::onPointerReleased(const PointerEvent *event) { return false; }
bool View::onPointerClicked(const PointerEvent *event) { return false; }
bool View::onPointerMoved(const PointerEvent *event) { return false; }

bool View::hasMouseInput() const { return false; }
bool View::hasMouseTracking() const { return false; }
bool View::onMousePressed(const MouseEvent *event) { return false; }
bool View::onMouseReleased(const MouseEvent *event) { return false; }
bool View::onMouseClicked(const MouseEvent *event) { return false; }
bool View::onMouseMoved(const MouseEvent *event) { return false; }

bool View::hasFingerInput() const { return false; }
bool View::onFingerPressed(const FingerEvent *event) { return false; }
bool View::onFingerReleased(const FingerEvent *event) { return false; }
bool View::onFingerClicked(const FingerEvent *event) { return false; }
bool View::onFingerMoved(const FingerEvent *event) { return false; }

bool View::hasWheelInput() const { return false; }
bool View::onWheelMoved(const WheelEvent *event) { return false; }

bool View::hasKeyInput() const { return false; }
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

bool View::fingerEvent(FingerEvent *event)
{
    const bool hpi = hasPointerInput();
    const bool hfi = hasFingerInput();

    if (hpi || hfi)
    {
        PointerEvent::PosGuard guard(&event->pos_, mapToLocal(event->pos()));

        if (event->action() == PointerAction::Pressed)
        {
            if (
                (hpi && onPointerPressed(event)) ||
                (hfi && onFingerPressed(event))
            ) {
                window()->touchTargets_->establish(event->fingerId(), this);
                return true;
            }
        }
        else if (event->action() == PointerAction::Released)
        {
            bool eaten =
                (hpi && onPointerReleased(event)) ||
                (hfi && onFingerReleased(event));

            if (containsLocal(event->pos())) {
                if (
                    (hpi && onPointerClicked(event)) ||
                    (hfi && onFingerClicked(event))
                )
                    eaten = true;
            }

            if (eaten) return true;
        }
        else if (event->action() == PointerAction::Moved)
        {
            if (
                (hpi && onPointerMoved(event)) ||
                (hfi && onFingerMoved(event))
            ) return true;
        }
    }

    for (auto pair: children_)
    {
        View *child = pair->value();

        if (
            (child->hasPointerInput() || child->hasFingerInput())  &&
            child->containsGlobal(event->pos())
        ) {
            if (child->fingerEvent(event))
                return true;
        }
    }

    return false;
}

bool View::mouseEvent(MouseEvent *event)
{
    bool hpi = hasPointerInput();
    bool hmi = hasMouseInput();

    if (hpi || hmi)
    {
        PointerEvent::PosGuard guard(&event->pos_, mapToLocal(event->pos()));

        if (event->action() == PointerAction::Pressed)
        {
            if (
                (hpi && onPointerPressed(event)) ||
                (hmi && onMousePressed(event))
            ) {
                window()->pointerTarget_ = this;
                return true;
            }
        }
        else if (event->action() == PointerAction::Released)
        {
            bool eaten =
                (hpi && onPointerReleased(event)) ||
                (hmi && onMouseReleased(event));

            if (containsLocal(event->pos())) {
                if (
                    (hpi && onPointerClicked(event)) ||
                    (hmi && onMouseClicked(event))
                )
                    eaten = true;
            }

            if (eaten) return true;
        }
        else if (event->action() == PointerAction::Moved)
        {
            if (event->button() != MouseButton::None || hasMouseTracking()) {
                if (
                    (hpi && onPointerMoved(event)) ||
                    (hmi && onMouseMoved(event))
                ) return true;
            }
        }
    }

    for (auto pair: children_)
    {
        View *child = pair->value();

        if (
            (child->hasPointerInput() || child->hasMouseInput()) &&
            child->containsGlobal(event->pos())
        ) {
            if (child->mouseEvent(event))
                return true;
        }
    }

    return false;
}

bool View::wheelEvent(WheelEvent *event)
{
    if (
        hasWheelInput() &&
        containsGlobal(event->mousePos())
    ) {
        if (onWheelMoved(event))
            return true;
    }

    for (auto pair: children_)
    {
        View *child = pair->value();

        if (child->wheelEvent(event))
            return true;
    }

    return false;
}

bool View::keyEvent(KeyEvent *event)
{
    if (hasKeyInput())
    {
        if (event->action() == KeyAction::Pressed)
        {
            if (onKeyPressed(event)) return true;
        }
        else if (event->action() == KeyAction::Released)
        {
            if (onKeyReleased(event)) return true;
        }
    }

    for (auto pair: children_)
    {
        if (pair->value()->keyEvent(event))
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

Window *View::window()
{
    if (!window_) {
        if (parent_)
            window_ = parent_->window();
    }
    return window_;
}

Image *View::image()
{
    if (!image_ || image_->size() != size())
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
