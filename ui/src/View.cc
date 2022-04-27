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
#include <cc/DEBUG>

namespace cc {

View::State::State()
{
    opacity([this]{ return hasParent() ? parent().opacity() : 1.; });

    angle([this]{
        return (hasParent() && parent().angle() != 0) ? parent().angle() : 0;
    });

    pivot([this]{
        return (hasParent() && parent().angle() != 0) ? parent().pivot() - pos() : size() / 2;
    });

    pos.onChanged([this]{ update(UpdateReason::Moved); });
    angle.onChanged([this]{ update(UpdateReason::Moved); });
    pivot.onChanged([this]{ update(UpdateReason::Moved); });
    scale.onChanged([this]{ update(UpdateReason::Moved); });
    opacity.onChanged([this]{ update(UpdateReason::Faded); });

    visible.onChanged([this]{
        for (View child: children_) child.me().visible = visible();

        if (decoration()) {
            decoration().visible(visible());
        }

        if (!visible()) {
            context_ = Object{};
            image_ = Image{};
            if (hasParent()) {
                parent().me().visibleChildren_.remove(self());
            }
            update(UpdateReason::Hidden);
        }
        else {
            if (hasParent()) {
                parent().me().visibleChildren_.insert(self());
            }
            paint();
            if (decoration()) decoration()->paint();
        }
    });

    decoration.onChanged([this]{
        decoration()->parent_ = this;
    });

    paint([this]{ if (isPainted()) polish(); });

    paint.onChanged([this]{
        update(UpdateReason::Changed);
    });

    paint.restrict([this]{
        paintTarget(this);
        return true;
    });
}

View::State::~State()
{
    if (layout_()) {
        // destroy the layout before releasing the children for efficiency
        layout_(Layout{});
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
    for (View h = self(); h.hasParent(); h = h.parent()) l += h.pos();
    return l;
}

Point View::State::mapToLocal(Point g) const
{
    for (View h = self(); h.hasParent(); h = h.parent()) g -= h.pos();
    return g;
}

Point View::State::mapToParent(const View &parent, Point l) const
{
    for (View h = self(); h != parent && h.hasParent(); h = h.parent()) l += h.pos();
    return l;
}

Point View::State::mapToChild(const View &child, Point l) const
{
    View s = self();
    for (View h = child; h != s && h.hasParent(); h = h.parent()) l -= h.pos();
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
        if (candidate.containsLocal(lc)) {
            if (candidate.is<Control>()) {
                control = candidate.as<Control>();
                if (control.delegate()) {
                    while (control.delegate())
                        control = control.delegate();
                }
            }
            else
                control = candidate.findControl(lc);
            break;
        }
    }
    return control;
}

bool View::State::isParentOf(const View &other) const
{
    View s = self();
    for (View h = other; h; h = h.parent()) {
        if (h == s) return true;
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
    for (View h = parent(); h; h = h.parent()) {
        if (h.paper())
            return h.paper();
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
        child.me().id_ = newId;
    }
    insertChild(child);
}

void View::State::insertChild(View child)
{
    child.me().parent_ = this;
    if (std::isnan(child.me().id_)) {
        child.me().id_ = nextId();
    }
    children_.insert(child);
    if (!visible()) {
        child.visible(false);
    }
    else if (child.visible()) {
        visibleChildren_.insert(child);
    }
    childReady(&child);
    ++childrenCount;
}

void View::State::removeChild(View child)
{
    children_.remove(child);
    if (child.visible()) {
        visibleChildren_.remove(child);
        child.visible(false);
    }
    child.me().id_ = 0;
    childDone(&child);
    --childrenCount;
}

void View::State::moveToTop(View child)
{
    if (children_.max() != child && children_.find(child))
    {
        child.visible(false);
        children_.remove(child);
        child.me().id_ = children_.max().id() + 1;
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
        child.me().id_ = children_.min().id() - 1;
        children_.insert(child);
        child.visible(true);
    }
}

cairo_surface_t *View::State::cairoSurface()
{
    return image().cairoSurface();
}

Image View::State::image()
{
    if (!image_ || (
        image_.width()  != std::ceil(size()[0]) ||
        image_.height() != std::ceil(size()[1])
    ))
        image_ = Image{int(std::ceil(size()[0])), int(std::ceil(size()[1]))};

    return image_;
}

View View::State::self() const
{
    return alias<View>(this);
}

Window View::State::window() const
{
    return window_() ? weak<Window>(window_()) : Window{};
}

bool View::State::isHandheld() const
{
    return (window_() && window().display()) ? window().display().isHandheld() : true;
}

void View::State::update(UpdateReason reason)
{
    if (!window()) return;

    if (!visible() && reason != UpdateReason::Hidden) return;

    window().me().addToFrame(UpdateRequest{reason, self()});
}

bool View::State::feedExposedEvent() const
{
    if (isPainted()) {
        paint();
        if (decoration()) decoration()->paint();
    }

    for (const View &child: visibleChildren_)
    {
        if (child.me().feedExposedEvent())
            return true;
    }

    return false;
}

bool View::State::feedEnterEvent() const
{
    onWindowEntered.emit();

    for (const View &child: visibleChildren_)
    {
        if (child.me().feedEnterEvent())
            return true;
    }

    return false;
}

bool View::State::feedLeaveEvent() const
{
    onWindowLeft.emit();

    for (const View &child: visibleChildren_)
    {
        if (child.me().feedLeaveEvent())
            return true;
    }

    return false;
}

bool View::State::feedFingerEvent(FingerEvent &event) const
{
    {
        PosGuard guard{event, mapToLocal(window().size() * event.pos())};

        if (event.action() == PointerAction::Pressed)
        {
            if (onPointerPressed.propagate(event) || onFingerPressed.propagate(event))
                return true;
        }
        else if (event.action() == PointerAction::Released)
        {
            if (onPointerReleased.propagate(event) || onFingerReleased.propagate(event))
                return true;
        }
        else if (event.action() == PointerAction::Moved)
        {
            if (onPointerMoved.propagate(event) || onFingerMoved.propagate(event))
                return true;
        }
    }

    for (const View &child: visibleChildren_)
    {
        if (child.containsGlobal(event.pos()))
        {
            if (child.me().feedFingerEvent(event))
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
            if (onPointerPressed.propagate(event) || onMousePressed.propagate(event))
                return true;
        }
        else if (event.action() == PointerAction::Released)
        {
            if (onPointerReleased.propagate(event) || onMouseReleased.propagate(event))
                return true;
        }
        else if (event.action() == PointerAction::Moved)
        {
            if (
                (event.button() != MouseButton::None && onPointerMoved.propagate(event)) ||
                onMouseMoved.propagate(event)
            )
                return true;
        }
    }

    for (const View &child: visibleChildren_)
    {
        if (child.containsGlobal(event.pos()))
        {
            if (child.me().feedMouseEvent(event))
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
        if (child.me().containsGlobal(event.mousePos()))
        {
            if (child.me().feedWheelEvent(event))
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
        if (child.me().feedKeyEvent(event))
            return true;
    }

    return false;
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

} // namespace cc::u
