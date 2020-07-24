/*
 * Copyright (C) 2017-2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/View>
#include <cc/ui/Application>
#include <cc/ui/StyleManager>
#include <cc/ui/Window>
#include <cc/ui/UpdateRequest>
#include <cc/ui/Control>

namespace cc {
namespace ui {

View::Instance::Instance()
{
    build >>[=]{
        if (parent()) {
            pos >>[=]{
                if (visible()) update(UpdateReason::Moved);
            };
        }
        else {
            pos->restrict([](Point&, Point) { return false; });
        }

        picture->schedule([=]{ update(UpdateReason::Changed); });

        angle >>[=]{ update(UpdateReason::Moved); };
        scale >>[=]{ update(UpdateReason::Moved); };

        visible >>[=]{
            for (int i = 0, n = childCount(); i < n; ++i)
                childAt(i)->visible = visible();
            if (!visible()) {
                context_ = nullptr;
                image_ = Image{};
                if (parent_)
                    parent_->visibleChildren_->remove(serial_);
                update(UpdateReason::Hidden);
            }
            else {
                if (parent())
                    parent()->visibleChildren_->insert(serial_, this);
                if (isPainted()) {
                    clear();
                    paint();
                    update(UpdateReason::Changed);
                }
            }
        };
    };
}

View::Instance::~Instance()
{
    layout_ = Layout{nullptr};
        // destroy the layout before releasing the children for efficiency
}

void View::Instance::disband()
{
    for (auto &item: children_)
        item->value()->disband();

    build->disband();
    paint->disband();

    visible->disband();
    moving->disband();
    paper->disband();

    pos->disband();
    size->disband();
    padding->disband();

    center->disband();
    angle->disband();
    scale->disband();
    childCount->disband();

    layout_ = Layout{nullptr};
}

Point View::Instance::mapToGlobal(Point l) const
{
    for (const Instance *h = this; h->parent_; h = h->parent_)
        l += h->pos();
    return l;
}

Point View::Instance::mapToLocal(Point g) const
{
    for (const Instance *h = this; h->parent_; h = h->parent_)
       g -= h->pos();
    return g;
}

Point View::Instance::mapToChild(const Instance *child, Point l) const
{
    for (const Instance *h = child; h != this && h->parent_; h = h->parent_)
        l -= h->pos();
    return l;
}

Point View::Instance::mapToParent(const Instance *parent, Point l) const
{
    for (const Instance *h = this; h != parent && h->parent_; h = h->parent_)
        l += h->pos();
    return l;
}

bool View::Instance::withinBounds(Point l) const
{
    return
        0 <= l[1] && l[1] < size()[1] &&
        0 <= l[0] && l[0] < size()[0];
}

View View::Instance::getChildAt(Point l) /// \todo replace by a safer version: bool lookupChildAt(Point l, View *view) const;
{
    for (auto pair: visibleChildren_) {
        View child = pair->value();
        if (child->containsLocal(mapToChild(child, l))) return child;
    }
    return View{nullptr};
}

Control View::Instance::getControlAt(Point l) /// \todo replace by a safer version: bool lookupControlAt(Point l, Control *view) const;
{
    for (auto pair: visibleChildren_) {
        View child = pair->value();
        if (child->containsLocal(mapToChild(child, l))) {
            Control control = child->as<Control>();
            if (control) {
                while (control->delegate())
                    control = control->delegate();
                return control;
            }
        }
    }
    return Control{nullptr};
}

bool View::Instance::isParentOf(const Instance *other) const
{
    for (const Instance *h = other; h; h = h->parent_) {
        if (h == this)
            return true;
    }
    return false;
}

bool View::Instance::isFullyVisibleIn(const Instance *other) const
{
    if (!other) return false;
    if (other == this) return true;
    if (!other->isParentOf(this)) return false;

    return
        other->withinBounds(mapToParent(other, Point{})) &&
        other->withinBounds(mapToParent(other, size() - Size{1, 1}));
}

void View::Instance::centerInParent()
{
    if (parent()) pos <<[=]{ return 0.5 * (parent()->size() - size()); };
    else pos = Point{};
}

/// \todo rename to basePaper
Color View::Instance::basePaper() const
{
    for (const Instance *h = parent(); h; h = h->parent()) {
        if (h->paper())
            return h->paper();
    }

    return style()->theme()->windowColor();
}

void View::Instance::inheritPaper()
{
    paper <<[=]{ return basePaper(); };
}

bool View::Instance::isOpaque() const
{
    return paper()->isOpaque();
}

bool View::Instance::isPainted() const
{
    return paper()->isValid() && size()[0] > 0 && size()[1] > 0;
}

bool View::Instance::isStatic() const
{
    return false; // FIXME
}

void View::Instance::clear(Color c)
{
    image()->clear(c->premultiplied());
}

void View::Instance::clear()
{
    clear(paper());
}

bool View::Instance::onPointerPressed(const PointerEvent *event) { return false; }
bool View::Instance::onPointerReleased(const PointerEvent *event) { return false; }
bool View::Instance::onPointerMoved(const PointerEvent *event) { return false; }

bool View::Instance::onMousePressed(const MouseEvent *event) { return false; }
bool View::Instance::onMouseReleased(const MouseEvent *event) { return false; }
bool View::Instance::onMouseMoved(const MouseEvent *event) { return false; }

bool View::Instance::onFingerPressed(const FingerEvent *event) { return false; }
bool View::Instance::onFingerReleased(const FingerEvent *event) { return false; }
bool View::Instance::onFingerMoved(const FingerEvent *event) { return false; }

bool View::Instance::onWheelMoved(const WheelEvent *event) { return false; }

bool View::Instance::onKeyPressed(const KeyEvent *event) { return false; }
bool View::Instance::onKeyReleased(const KeyEvent *event) { return false; }

bool View::Instance::onWindowExposed()
{
    update(UpdateReason::Exposed);
        // FIXME: double composition on startup?
    return false;
}

bool View::Instance::onWindowEntered() { return false; }
bool View::Instance::onWindowLeft() { return false; }

void View::Instance::update(UpdateReason reason)
{
    Window *w = window();
    if (!w) return;

    if (
        isPainted() &&
        (reason == UpdateReason::Changed || reason == UpdateReason::Resized)
    )
        picture();

    if (!visible() && reason != UpdateReason::Hidden) return;

    w->addToFrame(UpdateRequest::create(reason, this));
}

void View::Instance::childReady(View &child)
{
    if (layout_) layout_->childReady(child);
}

void View::Instance::childDone(View &child)
{
    if (layout_) layout_->childDone(child);
}

StylePlugin *View::Instance::style() const
{
    return StyleManager::instance()->activePlugin();
}

const Theme *View::Instance::theme() const
{
    return style()->theme();
}

Window *View::Instance::window() const
{
    if (!window_) {
        if (parent_)
            return parent_->window();
    }
    return window_;
}

Image::Instance *View::Instance::image()
{
    if (!image_ || (
        image_->width()  != std::ceil(size()[0]) ||
        image_->height() != std::ceil(size()[1])
    ))
        image_ = Image{int(std::ceil(size()[0])), int(std::ceil(size()[1]))};

    return image_;
}

void View::Instance::insertChild(View child)
{
    child->parent_ = this;
    child->serial_ = nextSerial();
    children_->insert(child->serial_, child);
    child->build();
    if (child->visible())
        visibleChildren_->insert(child->serial_, child);
    childCount += 1;
}

void View::Instance::removeChild(View child)
{
    View hook = child;
    children_->remove(child->serial_);
    if (child->visible())
        visibleChildren_->remove(child->serial_);
    child->serial_ = 0;
    childCount -= 1;
    childDone(child);
}

void View::Instance::adoptChild(View &parent, View &child)
{
    parent->insertChild(child);
}

bool View::Instance::feedFingerEvent(FingerEvent *event)
{
    {
        PointerEvent::PosGuard guard{event, mapToLocal(window()->size() * event->pos())};

        if (event->action() == PointerAction::Pressed)
        {
            if (onPointerPressed(event) || onFingerPressed(event))
                return true;
        }
        else if (event->action() == PointerAction::Released)
        {
            bool eaten = onPointerReleased(event) || onFingerReleased(event);

            if (Application{}->pressedControl()) {
                if (
                    Application{}->pressedControl()->onPointerClicked(event) ||
                    Application{}->pressedControl()->onFingerClicked(event)
                )
                    eaten = true;
            }

            if (eaten) return true;
        }
        else if (event->action() == PointerAction::Moved)
        {
            if (onPointerMoved(event) || onFingerMoved(event))
                return true;
        }
    }

    for (auto pair: visibleChildren_)
    {
        View child = pair->value();

        if (child->containsGlobal(event->pos()))
        {
            if (child->feedFingerEvent(event))
                return true;
        }
    }

    return false;
}

bool View::Instance::feedMouseEvent(MouseEvent *event)
{
    {
        PointerEvent::PosGuard guard{event, mapToLocal(event->pos())};

        if (event->action() == PointerAction::Pressed)
        {
            if (onPointerPressed(event) || onMousePressed(event))
                return true;
        }
        else if (event->action() == PointerAction::Released)
        {
            bool eaten = onPointerReleased(event) || onMouseReleased(event);

            if (Application{}->pressedControl()) {
                if (
                    Application{}->pressedControl()->onPointerClicked(event) ||
                    Application{}->pressedControl()->onMouseClicked(event)
                )
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
    }

    for (auto pair: visibleChildren_)
    {
        View child = pair->value();

        if (child->containsGlobal(event->pos()))
        {
            if (child->feedMouseEvent(event))
                return true;
        }
    }

    return false;
}

bool View::Instance::feedWheelEvent(WheelEvent *event)
{
    if (containsGlobal(event->mousePos()))
    {
        if (onWheelMoved(event))
            return true;
    }

    for (auto pair: visibleChildren_)
    {
        View child = pair->value();

        if (child->containsGlobal(event->mousePos()))
        {
            if (child->feedWheelEvent(event))
                return true;
        }
    }

    return false;
}

bool View::Instance::feedKeyEvent(KeyEvent *event)
{
    if (event->action() == KeyAction::Pressed)
    {
        if (onKeyPressed(event)) return true;
    }
    else if (event->action() == KeyAction::Released)
    {
        if (onKeyReleased(event)) return true;
    }

    for (auto pair: visibleChildren_)
    {
        View child = pair->value();

        if (child->feedKeyEvent(event))
            return true;
    }

    return false;
}

bool View::Instance::feedExposedEvent()
{
    if (onWindowExposed()) return true;

    for (auto pair: visibleChildren_)
    {
        View child = pair->value();

        if (child->feedExposedEvent())
            return true;
    }

    return false;
}

bool View::Instance::feedEnterEvent()
{
    if (onWindowEntered()) return true;

    for (auto pair: visibleChildren_)
    {
        View child = pair->value();

        if (child->feedLeaveEvent())
            return true;
    }

    return false;
}

bool View::Instance::feedLeaveEvent()
{
    if (onWindowLeft()) return true;

    for (auto pair: visibleChildren_)
    {
        View child = pair->value();

        if (child->feedLeaveEvent())
            return true;
    }

    return false;
}

uint64_t View::Instance::nextSerial() const
{
    return (children_->count() > 0) ? children_->at(children_->count() - 1)->key() + 1 : 1;
}

void View::Instance::polish(Window *window)
{
    for (auto pair: visibleChildren_)
    {
        View child = pair->value(); /// \todo const View &

        child->polish(window);
    }

    picture();

    window->addToFrame(UpdateRequest::create(UpdateReason::Changed, this));
}

cairo_surface_t *View::Instance::cairoSurface() const
{
    return const_cast<Instance *>(this)->image()->cairoSurface();
}

}} // namespace cc::ui
