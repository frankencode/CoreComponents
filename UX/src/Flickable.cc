/*
 * Copyright (C) 2020-2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Flickable>
#include <cc/Application>
#include <cc/InputControl>
#include <cc/PosGuard>
#include <cc/Timer>
#include <cc/DisplayManager>
#include <cc/System>

namespace cc {

Flickable::State::State(const View &carrier):
    carrier_{carrier},
    timer_{1./DisplayManager{}.refreshRate()}
{
    size([this]{ return hasParent() ? parent().size() : Size{}; });

    size.onChanged([this]{
        positionCarrierOnResize();
        keepFocusControlInView();
    });

    timer_.onTimeout([this]{
        if (timerMode_ == TimerMode::Flying) carrierFly();
        else if (timerMode_ == TimerMode::Bouncing) carrierBounce();
        else if (timerMode_ == TimerMode::Traversing) carrierTraverse();
    });

    focusControl([this]{
        return Application{}.focusControl();
    });

    focusControl.onChanged([this]{
        keepFocusControlInView();
    });

    if (!carrier_.hasParent()) View::State::insertChild(carrier_);

    onPointerClicked([this](const PointerEvent &event)
    {
        return !carrierAtRest_;
    });

    onPointerPressed([this](const PointerEvent &event)
    {
        dragStart_ = event.pos();
        isDragged_ = false;
        speed_ = Point{};

        if (timer_.isActive()) {
            wasFlying_ = true;
            timerStop();
            carrier_.moving(false);
            Application{}.pressedControl(self());
        }
        else {
            wasFlying_ = false;
            return false;
        }

        overwrittenControl_ = carrier_.findControl(mapToChild(carrier_, event.pos()));
        if (overwrittenControl_) overwrittenControl_->pressedOverwrite(true);

       return true;
    });

    onPointerReleased([this](const PointerEvent &event)
    {
        if (overwrittenControl_) {
            overwrittenControl_->pressedOverwrite(false);
            overwrittenControl_ = Control{};
        }

        if (isDragged_) {
            if (event.time() - lastDragTime_ > minHoldTime()) speed_ = Point{};
            isDragged_ = false;
        }

        carrierAtRest_ = false;

        if (carrierInsideBoundary())
            carrierBounceStart();
        else if (speed_ != Step{})
            carrierFlyStart();
        else if (!wasFlying_)
            carrierAtRest_ = true;

        return true;
    });

    onPointerMoved([this](const PointerEvent &event)
    {
        if (!isDragged_ && Application{}.pointerIsDragged(event, dragStart_)) {
            isDragged_ = true;
            carrierOrigin_ = carrier_.pos();
            lastDragTime_ = 0;
            Application{}.pressedControl(self());
        }

        if (isDragged_) {
            double t = event.time();
            if (lastDragTime_ > 0 && t != lastDragTime_)
                speed_ = (lastDragPos_ - event.pos()) / (t - lastDragTime_);

            lastDragTime_ = t;
            lastDragPos_ = event.pos();

            Step d = event.pos() - dragStart_;
            Point p = carrierOrigin_ + d;

            carrier_.pos(carrierStep(p, boundary()));
        }

        return true;
    });

    onWheelMoved([this](const WheelEvent &event)
    {
        if (Application{}.focusControl())
            Application{}.focusControl(Control{});

        if (timerMode_ == TimerMode::Bouncing || timerMode_ == TimerMode::Traversing)
            carrierStop();

        if (timerMode_ != TimerMode::Stopped)
            return true;

        carrier_.pos(carrierStep(carrier_.pos() + event.wheelStep() * wheelGranularity(), wheelBouncing() ? boundary() : 0));

        if (carrierInsideBoundary())
            carrierBounceStart();

        return true;
    });
}

Flickable::State::~State()
{
    timer_.stop();
}

void Flickable::State::preheat()
{}

void Flickable::State::positionCarrierOnResize()
{
    carrier_.pos(carrierStep(carrier_.pos()));
}

void Flickable::State::keepFocusControlInView()
{
    if (!carrier_) return;
    View target;
    for (View view = focusControl(); view; view = view.parent()) {
        if (view.parent() == carrier_) {
            target = view;
            break;
        }
    }
    if (target) {
        const double margin = dp(16);
        if (target.laysInsideOf(self(), margin)) return;

        Point topLeft = target.mapToParent(self(), Point{-margin, -margin});
        if (!withinBounds(topLeft)) {
            Step step;
            if (topLeft[0] < 0) step[0] = -topLeft[0];
            if (topLeft[1] < 0) step[1] = -topLeft[1];
            if (step != Step{}) {
                carrierTraverseStart(step);
                return;
            }
        }

        Point bottomRight = topLeft + target.size() + 2 * Size{margin, margin};
        if (!withinBounds(bottomRight)) {
            Step step;
            if (bottomRight[0] > size()[0]) step[0] = size()[0] - bottomRight[0];
            if (bottomRight[1] > size()[1]) step[1] = size()[1] - bottomRight[1];
            if (step != Step{}) {
                carrierTraverseStart(step);
                return;
            }
        }
    }
}

bool Flickable::State::carrierInsideBoundary() const
{
    double x = carrier_.pos()[0];
    double y = carrier_.pos()[1];
    double w = carrier_.size()[0];
    double h = carrier_.size()[1];

    return
        (w > size()[0] && (x > 0 || x + w < size()[0])) ||
        (h > size()[1] && (y > 0 || y + h < size()[1]));
}

Point Flickable::State::carrierStep(Point p, double b)
{
    double x = p[0];
    double y = p[1];
    double w = carrier_.size()[0];
    double h = carrier_.size()[1];

    if (w > size()[0]) {
        if (x > b) x = b;
        else if (x + w < size()[0] - b) x = size()[0] - b - w;
    }
    else x = 0;

    if (h > size()[1]) {
        if (y > b) y = b;
        else if (y + h < size()[1] - b) y = size()[1] - b - h;
    }
    else y = 0;

    return Point{x, y};
}

void Flickable::State::carrierFlyStart()
{
    timerMode_ = TimerMode::Flying;
    timer_.start();
    lastTime_ = timer_.startTime();
    double v = speed_.abs();
    if (v > maxSpeed()) speed_ *= maxSpeed() / v;
    releaseSpeedMagnitude_ = speed_.abs();
    carrier_.moving(true);
}

void Flickable::State::carrierBounceStart()
{
    timerMode_ = TimerMode::Bouncing;
    timer_.start();
    startPos_ = carrier_.pos();
    finalPos_ = carrierStep(carrier_.pos());
    carrier_.moving(true);
}

void Flickable::State::carrierTraverseStart(Step distance)
{
    timerMode_ = TimerMode::Traversing;
    timer_.start();
    startPos_ = carrier_.pos();
    finalPos_ = carrierStep(carrier_.pos() + distance);
    carrier_.moving(true);
}

void Flickable::State::carrierStop()
{
    carrier_.moving(false);
    carrier_.pos(finalPos_);
    timerStop();
    carrierStopped();
}

void Flickable::State::timerStop()
{
    timer_.stop();
    timerMode_ = TimerMode::Stopped;
}

static double fastAbs(Vector<double, 2> v)
{
    double a = 0;
    if (v[0] == 0) a = std::abs(v[1]);
    else if (v[1] == 0) a = std::abs(v[0]);
    else a = v.abs();
    return a;
}

void Flickable::State::carrierFly()
{
    if (speed_ == Step{}) {
        timerStop();
        return;
    }

    double t = System::now();
    double T = t - lastTime_;
    lastTime_ = t;
    Step d = -speed_ * T;
    double va = fastAbs(speed_);
    double vb = va - (T / maxFlyTime()) * releaseSpeedMagnitude_;
    if (vb <= 0) vb = 0;
    speed_ *= vb / va;

    Point pa = carrier_.pos();
    Point pb = carrierStep(carrier_.pos() + d, boundary());
    if (pa == pb || speed_ == Step{}) {
        timerStop();
        if (carrierInsideBoundary()) {
            carrierBounceStart();
        }
        else {
            carrier_.moving(false);
            carrierStopped();
        }
    }
    else
        carrier_.pos(pb);
}

void Flickable::State::carrierBounce()
{
    const double t0 = timer_.startTime();
    const double t1 = t0 + maxBounceTime();
    const double t = System::now();

    if (t >= t1) {
        carrierStop();
        return;
    }

    double s = Easing::OutBounce((t - t0) / (t1 - t0));
    carrier_.pos((1 - s) * startPos_ + s * finalPos_);
}

void Flickable::State::carrierTraverse()
{
    const double t0 = timer_.startTime();
    const double t1 = t0 + traverseTime();
    const double t = System::now();

    if (t >= t1) {
        carrierStop();
        return;
    }

    double s = Easing::InQuad((t - t0) / (t1 - t0));
    carrier_.pos((1 - s) * startPos_ + s * finalPos_);
}

void Flickable::State::carrierStopped()
{
    preheat();
}

Flickable::Flickable():
    Organizer{onDemand<State>}
{}

Flickable::Flickable(double width, double height, const View &carrier):
    Organizer{new State{carrier}}
{
    size(Size{width, height});
}

Flickable::Flickable(const View &carrier):
    Organizer{new State{carrier}}
{}

Flickable &Flickable::associate(Out<Flickable> self)
{
    return View::associate<Flickable>(self);
}

} // namespace cc
