/*
 * Copyright (C) 2017-2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/List>
#include <cc/ui/StyleManager>
#include <cc/ui/Window>
#include <cc/ui/Image>
#include <cc/ui/Control>
#include <cc/ui/View>

namespace cc {
namespace ui {

View::View(View *parent):
    serial_(0),
    window_(0),
    parent_(0),
    children_(Children::create()),
    visibleChildren_(Children::create())
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

    picture->schedule([=]{ update(UpdateReason::Changed); });

    angle->connect([=]{ update(UpdateReason::Moved); });
    scale->connect([=]{ update(UpdateReason::Moved); });

    visible->connect([=]{
        for (int i = 0, n = childCount(); i < n; ++i)
            childAt(i)->visible = visible();
        if (!visible()) {
            context_ = nullptr;
            image_ = nullptr;
            if (parent_)
                parent_->visibleChildren_->remove(serial_);
            update(UpdateReason::Hidden);
        }
        else {
            if (parent_)
                parent_->visibleChildren_->insert(serial_, this);
            if (isPainted()) {
                clear();
                paint();
                update(UpdateReason::Changed);
            }
        }
    });
}

View::~View()
{
    organizer_ = nullptr;
        // destroy the organizer before releasing the children for efficiency
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
    for (auto pair: visibleChildren_) {
        View *child = pair->value();
        if (child->containsGlobal(g))
            return child->getTopViewAt(g);
    }
    return this;
}

Control *View::getTopControlAt(Point g)
{
    View *view = getTopViewAt(g);
    while (view && !Object::cast<Control *>(view)) view = view->parent();
    if (!view) return nullptr;
    Control *control = Object::cast<Control *>(view);
    if (!control->delegate()) return control;
    return control->delegate();
}

bool View::isParentOfOrEqual(View *other) const
{
    for (View *view = other; view; view = view->parent()) {
        if (view == this)
            return true;
    }
    return false;
}

void View::centerInParent()
{
    if (parent()) pos->bind([=]{ return 0.5 * (parent()->size() - size()); });
    else pos = Point{};
}

void View::inheritPaper()
{
    for (View *view = parent(); view; view = view->parent()) {
        if (view->paper()) {
            paper->bind([=]{ return view->paper(); });
            return;
        }
    }

    paper->bind([=]{ return style()->theme()->windowColor(); });
}

bool View::isOpaque() const
{
    return paper()->isOpaque();
}

bool View::isPainted() const
{
    return paper()->isValid();
}

bool View::isStatic() const
{
    return true;
}

void View::clear(Color color)
{
    image()->clear(color->premultiplied());
}

void View::clear()
{
    clear(paper());
}

void View::paint()
{}

bool View::onPointerPressed(const PointerEvent *event) { return false; }
bool View::onPointerReleased(const PointerEvent *event) { return false; }
bool View::onPointerMoved(const PointerEvent *event) { return false; }

bool View::onMousePressed(const MouseEvent *event) { return false; }
bool View::onMouseReleased(const MouseEvent *event) { return false; }
bool View::onMouseMoved(const MouseEvent *event) { return false; }

bool View::onFingerPressed(const FingerEvent *event) { return false; }
bool View::onFingerReleased(const FingerEvent *event) { return false; }
bool View::onFingerMoved(const FingerEvent *event) { return false; }

bool View::onWheelMoved(const WheelEvent *event) { return false; }

bool View::onKeyPressed(const KeyEvent *event) { return false; }
bool View::onKeyReleased(const KeyEvent *event) { return false; }

bool View::onExposed() { return false; }

void View::update(UpdateReason reason)
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

void View::childReady(View *child)
{
    if (organizer_) organizer_->childReady(child);
}

void View::childDone(View *child)
{
    if (organizer_) organizer_->childDone(child);
}

Application *View::app() const
{
    return Application::instance();
}

StylePlugin *View::style() const
{
    return StyleManager::instance()->activePlugin();
}

const Theme *View::theme() const
{
    return style()->theme();
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
    if (!image_ || (
        image_->width()  != ::ceil(size()[0]) ||
        image_->height() != ::ceil(size()[1])
    ))
        image_ = Image::create(::ceil(size()[0]), ::ceil(size()[1]));

    return image_;
}

void View::insertChild(View *child)
{
    child->parent_ = this;
    child->serial_ = nextSerial();
    children_->insert(child->serial_, child);
    if (child->visible())
        visibleChildren_->insert(child->serial_, child);
    childCount += 1;
}

void View::removeChild(View *child)
{
    Ref<View> hook = child;
    children_->remove(child->serial_);
    if (child->visible())
        visibleChildren_->remove(child->serial_);
    child->serial_ = 0;
    childCount -= 1;
    childDone(child);
}

void View::adoptChild(View *parent, View *child)
{
    parent->insertChild(child);
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

        if (app()->pressedControl()) {
            if (
                app()->pressedControl()->onPointerClicked(event) ||
                app()->pressedControl()->onFingerClicked(event)
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

    for (auto pair: visibleChildren_)
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

        if (app()->pressedControl()) {
            if (
                app()->pressedControl()->onPointerClicked(event) ||
                app()->pressedControl()->onMouseClicked(event)
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

    for (auto pair: visibleChildren_)
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

    for (auto pair: visibleChildren_)
    {
        View *child = pair->value();

        if (child->containsGlobal(event->mousePos()))
        {
            if (child->feedWheelEvent(event))
                return true;
        }
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

    for (auto pair: visibleChildren_)
    {
        View *child = pair->value();

        if (child->feedKeyEvent(event))
            return true;
    }

    return false;
}

bool View::feedExposedEvent()
{
    if (onExposed()) return true;

    for (auto pair: visibleChildren_)
    {
        View *child = pair->value();

        if (child->feedExposedEvent())
            return true;
    }

    return false;
}

void View::init()
{
    if (parent()) parent()->childReady(this);
}

uint64_t View::nextSerial() const
{
    return (children_->count() > 0) ? children_->keyAt(children_->count() - 1) + 1 : 1;
}

void View::polish(Window *window)
{
    for (auto pair: visibleChildren_)
    {
        View *child = pair->value();

        child->polish(window);
    }

    picture();

    window->addToFrame(UpdateRequest::create(UpdateReason::Changed, this));
}

cairo_surface_t *View::cairoSurface() const
{
    return const_cast<View *>(this)->image()->cairoSurface();
}

}} // namespace cc::ui
