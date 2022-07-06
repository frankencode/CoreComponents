/*
 * Copyright (C) 2020-2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/View>
#include <cc/Application>
#include <cc/Window>
#include <cc/Control>
#include <cc/PosGuard>

namespace cc {

View::State::State()
{
    visible([this]{ return parent_() ? parent_()->visible() : true; });

    opacity([this]{ return parent_() ? parent_()->opacity() : 1.; });

    angle([this]{
        return (parent_() && parent_()->angle() != 0) ? parent_()->angle() : 0;
    });

    pivot([this]{
        return (parent_() && parent_()->angle() != 0) ? parent_()->pivot() - pos() : size() / 2;
    });

    pos.onChanged([this]{ update(UpdateReason::Moved); });
    angle.onChanged([this]{ update(UpdateReason::Moved); });
    // pivot.onChanged([this]{ update(UpdateReason::Moved); }); // FIXME
    scale.onChanged([this]{ update(UpdateReason::Moved); });
    opacity.onChanged([this]{ update(UpdateReason::Faded); });

    visible.onChanged([this]{
        if (decoration()) {
            decoration()->visible = visible();
        }

        if (!visible()) {
            context_ = Object{};
            image_ = Image{};
            if (hasParent()) {
                parent()->visibleChildren_.remove(self());
            }
            update(UpdateReason::Hidden);
        }
        else {
            if (hasParent()) {
                parent()->visibleChildren_.insert(self());
            }
            paint();
        }
    });

    decoration.onChanged([this]{
        decoration()->parent_ = this;
    });

    paint([this]{ if (isPainted()) polish(); });

    paint.onChanged([this]{
        if (isPremultiplied() && isPainted() && image_) image_.normalize();
        if (decoration()) decoration()->paint();
        update(UpdateReason::Changed);
    });
}

View::State::~State()
{
    if (layout_()) {
        // Disengage the layout before destroying the children for efficiency.
        layout_()->view_(nullptr);
    }
}

double View::State::expandableWidth(double width) const
{
    double maxWidth = 0;
    if (
        hasParent() &&
        parent().layout() &&
        !parent().layout().isManaged() &&
        parent().layout().orientation() == Orientation::Vertical
    ) {
        maxWidth = parent().innerWidth();
    }

    return width < maxWidth ? maxWidth : width;
}

Point View::State::mapToGlobal(Point l) const
{
    for (const View::State *h = this; h->parent_(); h = h->parent_()) l += h->pos();
    return l;
}

Point View::State::mapToLocal(Point g) const
{
    for (const View::State *h = this; h->parent_(); h = h->parent_()) g -= h->pos();
    return g;
}

Point View::State::mapToParent(const View &parent, Point l) const
{
    for (const View::State *h = this; h != &parent.me() && h->parent_(); h = h->parent_()) l += h->pos();
    return l;
}

Point View::State::mapToChild(const View &child, Point l) const
{
    for (const View::State *h = &child.me(); h != this && h->parent_(); h = h->parent_()) l -= h->pos();
    return l;
}

View View::State::findChild(Point l) const
{
    for (auto pos = visibleChildren_.tail(); pos; --pos) {
        View candidate = visibleChildren_.at(pos);
        if (candidate.containsLocal(mapToChild(candidate, l)))
            return candidate;
    }
    return View{};
}

Control View::State::findControl(Point l) const
{
    Control control;
    for (auto pos = visibleChildren_.tail(); pos; --pos) {
        View candidate = visibleChildren_.at(pos);
        Point lc = mapToChild(candidate, l);
        if (candidate.childrenRect().contains(lc)) {
            if (candidate.is<Control>()) {
                if (candidate.as<Control>().delegate()) {
                    control = candidate.as<Control>();
                    while (control.delegate()) {
                        control = control.delegate();
                    }
                }
                else if (candidate.as<Control>().cascade()) {
                    control = candidate.findControl(lc);
                    if (!control && candidate.containsLocal(lc)) {
                        control = candidate.as<Control>();
                    }
                }
                else if (candidate.containsLocal(lc)) {
                    control = candidate.as<Control>();
                }
            }
            else {
                control = candidate.findControl(lc);
            }
            break;
        }
    }
    return control;
}

bool View::State::isParentOf(const View &other) const
{
    if (!other) return false;
    if (other == this) return true;

    for (
        const View::State *h = &other.me();
        h;
        h = h->parent_()
    ) {
        if (h == this) return true;
    }
    return false;
}

bool View::State::laysInsideOf(const View &other, double margin) const
{
    if (!other) return false;

    return
        other.containsGlobal(mapToGlobal(Point{-margin, -margin})) &&
        other.containsGlobal(mapToGlobal(size() - Size{1 - margin, 1 - margin}));
}

Color View::State::basePaper() const
{
    for (const View::State *h = parent_(); h; h = h->parent_()) {
        if (h->paper()) return h->paper();
    }

    return theme().windowColor();
}

void View::State::childReady(InOut<View> child)
{
    child()->settled();
}

void View::State::childDone(InOut<View> child)
{}

void View::State::settled()
{}

void View::State::addBelow(const View &child)
{
    View{child}->id_ = nextBelowId();
    insertChild(child);
}

void View::State::insertAt(Locator target, View child)
{
    if (target) {
        View sibling1 = children_.at(target);
        View sibling2;
        --target;
        if (target) sibling2 = children_.at(target);
        double newId = sibling1.id();
        if (sibling2)
            newId = (newId + sibling2.id()) / 2;
        else
            --newId;
        child->id_ = newId;
    }
    insertChild(child);
}

void View::State::insertAt(long index, const View &child)
{
    if (children_.count() == 0) insertChild(child);
    else insertAt(children_.from(index), child);
}

void View::State::insertChild(View child)
{
    child->parent_ = this;
    if (std::isnan(child->id_)) {
        child->id_ = nextAboveId();
    }
    children_.insert(child);
    if (child.visible()) {
        visibleChildren_.insert(child);
    }
    childReady(&child);
    ++childrenCount;
    if (hasWindow() && child.visible()) {
        child->feedExposedEvent();
    }
}

void View::State::removeChild(View child)
{
    children_.remove(child);
    childDone(&child);
    --childrenCount;

    Application{}.postEvent([parent=alias<View>(this), child]() mutable {
        Application{}.disengage(child);
        if (child.visible()) parent->visibleChildren_.remove(child);
    });
}

void View::State::moveToTop(View child)
{
    if (children_.max() != child && children_.find(child))
    {
        child.visible(false);
        children_.remove(child);
        child->id_ = children_.max().id() + 1;
        children_.insert(child);
        child.visible(true);
    }
}

void View::State::moveToBottom(View child)
{
    if (children_.min() != child && children_.find(child))
    {
        child.visible(false);
        children_.remove(child);
        child->id_ = children_.min().id() - 1;
        children_.insert(child);
        child.visible(true);
    }
}

cairo_surface_t *View::State::cairoSurface()
{
    return image().cairoSurface();
}

Image &View::State::image()
{
    if (!image_ || (
        image_.width()  != std::ceil(size()[0]) ||
        image_.height() != std::ceil(size()[1])
    )) {
        image_ = Image{size()};
    }
    return image_;
}

View View::State::self() const
{
    return Object::alias<View>(this); // FIXME: weak?!
}

bool View::State::hasWindow() const
{
    if (!parent_()) return window_();
    if (!window_()) return parent_()->window_();
    return window_();
}


Window View::State::window() const
{
    if (!window_() && parent_()) return parent_()->window();
    return window_() ? Object::weak<Window>(window_()) : Window{};
}

bool View::State::sizing() const
{
    return !hasWindow() || window().sizing();
}

bool View::State::isHandheld() const
{
    return (window_() && window().display()) ? window().display().isHandheld() : true;
}

void View::State::update(UpdateReason reason)
{
    if (!window()) return;

    if (!visible() && reason != UpdateReason::Hidden) return;

    window()->addToFrame(UpdateRequest{reason, self()});
}

bool View::State::feedExposedEvent() const
{
    if (isPainted()) {
        paint();
        if (decoration()) decoration()->paint();
    }

    for (const View &child: visibleChildren_)
    {
        if (child->feedExposedEvent())
            return true;
    }

    return false;
}

bool View::State::feedEnterEvent() const
{
    onWindowEntered.emit();

    for (const View &child: visibleChildren_)
    {
        if (child->feedEnterEvent())
            return true;
    }

    return false;
}

bool View::State::feedLeaveEvent() const
{
    onWindowLeft.emit();

    for (const View &child: visibleChildren_)
    {
        if (child->feedLeaveEvent())
            return true;
    }

    return false;
}

bool View::State::feedFingerEvent(FingerEvent &event) const
{
    Point eventPos = window().size() * event.pos();
    {
        PosGuard guard{event, mapToLocal(eventPos)};

        if (event.action() == PointerAction::Pressed)
        {
            if (onPointerPressed(event) || onFingerPressed(event))
                return true;
        }
        else if (event.action() == PointerAction::Released)
        {
            if (onPointerReleased(event) || onFingerReleased(event))
                return true;
        }
        else if (event.action() == PointerAction::Moved)
        {
            if (onPointerMoved(event) || onFingerMoved(event))
                return true;
        }
    }

    for (const View &child: visibleChildren_)
    {
        if (child->containsGlobal(eventPos))
        {
            if (child->feedFingerEvent(event))
                return true;
        }
    }

    return false;
}

bool View::State::feedMouseEvent(MouseEvent &event) const
{
    {
        PosGuard guard{event, mapToLocal(event.pos())};

        if (event.action() == PointerAction::Pressed)
        {
            if (onPointerPressed(event) || onMousePressed(event))
                return true;
        }
        else if (event.action() == PointerAction::Released)
        {
            if (onPointerReleased(event) || onMouseReleased(event))
                return true;
        }
        else if (event.action() == PointerAction::Moved)
        {
            if (
                (event.button() != MouseButton::None && onPointerMoved(event)) ||
                onMouseMoved(event)
            )
                return true;
        }
    }

    for (const View &child: visibleChildren_)
    {
        if (child->containsGlobal(event.pos()))
        {
            if (child->feedMouseEvent(event))
                return true;
        }
    }

    return false;
}

bool View::State::feedWheelEvent(WheelEvent &event) const
{
    if (containsGlobal(event.mousePos()))
    {
        if (onWheelMoved.propagate(event))
            return true;
    }

    for (const View &child: visibleChildren_)
    {
        if (child->containsGlobal(event.mousePos()))
        {
            if (child->feedWheelEvent(event))
                return true;
        }
    }

    return false;
}

bool View::State::feedKeyEvent(KeyEvent &event) const
{
    if (event.action() == KeyAction::Pressed)
    {
        if (onKeyPressed.propagate(event)) return true;
    }
    else if (event.action() == KeyAction::Released)
    {
        if (onKeyReleased.propagate(event)) return true;
    }

    for (const View &child: visibleChildren_)
    {
        if (child->feedKeyEvent(event))
            return true;
    }

    return false;
}

void View::pop()
{
    Application{}.postEvent([view=*this]() mutable {
        if (!view) return;
        View target = view.visibleChildren().last();
        if (!target) return;
        target.visible(false);
        view.remove(target);
    });
}

Rect View::childrenRect() const
{
    double x0 = 0, x1 = width();
    double y0 = 0, y1 = height();

    for (const View &child: visibleChildren()) {
        Rect rect = child.childrenRect();
        rect.pos(
            child.pos()
            - Point{child.padding().left(), child.padding().top()}
        );
        rect.size(
            rect.size()
            + Size{
                child.padding().left() + child.padding().right(),
                child.padding().top() + child.padding().bottom()
            }
        );
        if (rect.x0() < x0) x0 = rect.x0();
        if (x1 < rect.x1()) x1 = rect.x1();
        if (rect.y0() < y0) y0 = rect.y0();
        if (y1 < rect.y1()) y1 = rect.y1();
    }

    return Rect{x0, y0, x1, y1};
}

void View::show()
{
    if (!hasParent()) {
        if (!hasWindow()) Window{*this}.show();
        else window().show();
    }
    else {
        if (!visible()) visible(true);
    }
}

int View::run()
{
    show();
    return Application{}.run();
}

Control View::findControl(Point l) const
{
    return me().findControl(l);
}

Window View::window() const
{
    return me().window();
}

void View::renderTo(Image &image) const
{
    assert(hasWindow());
    if (hasWindow()) window().renderViewToImage(*this, image);
}

} // namespace cc::u
