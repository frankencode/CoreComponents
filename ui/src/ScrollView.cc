/*
 * Copyright (C) 2018-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/debug> // DEBUG
#include <cc/math>
#include <cc/System>
#include <cc/ui/easing>
#include <cc/ui/Timer>
#include <cc/ui/InputField>
#include <cc/ui/ScrollView>

namespace cc {
namespace ui {

Ref<ScrollView> ScrollView::create(View *parent)
{
    return Object::create<ScrollView>(parent);
}

ScrollView::ScrollView(View *parent):
    Control{parent},
    carrier_{nullptr},
    isDragged_{false},
    timer_{Timer::create(1./60)}
{
    if (parent) size->bind([=]{ return parent->size(); });
}

ScrollView::~ScrollView()
{}

void ScrollView::init()
{
    carrier_ = addCarrier();

    size->connect([=]{
        positionCarrierOnResize();
        keepFocusControlInView();
    });

    timer_->triggered->connect([=]{
        if (timerMode_ == TimerMode::Flying) carrierFly();
        else if (timerMode_ == TimerMode::Bouncing) carrierBounce();
        else if (timerMode_ == TimerMode::Traversing) carrierTraverse();
    });

    focusControl->bind([]{
        return Application::instance()->focusControl();
    });

    focusControl->connect([=]{
        keepFocusControlInView();
    });
}

View *ScrollView::addCarrier()
{
    return add<View>();
}

void ScrollView::preheat()
{}

void ScrollView::positionCarrierOnResize()
{
    carrier_->pos = carrierStep(carrier_->pos());
}

void ScrollView::resetCarrier()
{
    timerStop();
    removeChild(carrier_);
    carrier_ = addCarrier();
}

void ScrollView::insertChild(View *child)
{
    if (carrier_)
        adoptChild(carrier_, child);
    else
        View::insertChild(child);
}

bool ScrollView::onPointerClicked(const PointerEvent *event)
{
    if (!carrierAtRest_) return true;

    Control *control = getControlAt(event->pos());
    if (control) {
        PointerEvent::PosGuard guard{const_cast<PointerEvent *>(event), mapToChild(control, event->pos())};
        return control->onPointerClicked(event);
    }

    return true;
}

bool ScrollView::onPointerPressed(const PointerEvent *event)
{
    dragStart_ = event->pos();
    isDragged_ = false;
    speed_ = Point{};

    if (timer_->isActive()) {
        wasFlying_ = true;
        timerStop();
        carrier_->moving = false;
    }
    else {
        wasFlying_ = false;
        Control *control = carrier()->getControlAt(mapToChild(carrier(), event->pos()));
        if (control) {
            app()->pressedControl = control;
            PointerEvent::PosGuard guard{const_cast<PointerEvent *>(event), mapToChild(control, event->pos())};
            return control->onPointerPressed(event);
        }
    }

    return true;
}

bool ScrollView::onPointerReleased(const PointerEvent *event)
{
    if (isDragged_) {
        if (event->time() - lastDragTime_ > minHoldTime()) speed_ = Point{};
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
}

bool ScrollView::onPointerMoved(const PointerEvent *event)
{
    if (!isDragged_ && Application::instance()->pointerIsDragged(event, dragStart_)) {
        isDragged_ = true;
        carrierOrigin_ = carrier_->pos();
        lastDragTime_ = 0;
        Application::instance()->pressedControl = nullptr;
    }

    if (isDragged_) {
        double t = event->time();
        if (lastDragTime_ > 0)
            speed_ = (lastDragPos_ - event->pos()) / (t - lastDragTime_);
        lastDragTime_ = t;
        lastDragPos_ = event->pos();

        Step d = event->pos() - dragStart_;
        Point p = carrierOrigin_ + d;

        carrier_->pos = carrierStep(p, boundary());
    }

    return true;
}

bool ScrollView::onWheelMoved(const WheelEvent *event)
{
    if (Application::instance()->focusControl())
        Application::instance()->focusControl = nullptr;

    if (timerMode_ == TimerMode::Bouncing || timerMode_ == TimerMode::Traversing)
        carrierStop();

    if (timerMode_ != TimerMode::Stopped)
        return true;

    carrier_->pos = carrierStep(carrier_->pos() + event->wheelStep() * wheelGranularity(), wheelBouncing() ? boundary() : 0);

    if (carrierInsideBoundary())
        carrierBounceStart();

    return true;
}

bool ScrollView::onWindowLeft()
{
    if (carrierInsideBoundary())
        carrierBounceStart();

    return false;
}

bool ScrollView::carrierInsideBoundary() const
{
    double x = carrier_->pos()[0];
    double y = carrier_->pos()[1];
    double w = carrier_->size()[0];
    double h = carrier_->size()[1];

    return
        (w > size()[0] && (x > 0 || x + w < size()[0])) ||
        (h > size()[1] && (y > 0 || y + h < size()[1]));
}

Point ScrollView::carrierStep(Point p, double b)
{
    double x = p[0];
    double y = p[1];
    double w = carrier_->size()[0];
    double h = carrier_->size()[1];

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

    return Point{ x, y };
}

void ScrollView::carrierFlyStart()
{
    timerMode_ = TimerMode::Flying;
    timer_->start();
    lastTime_ = timer_->startTime();
    double v = abs(speed_);
    if (v > maxSpeed()) speed_ *= maxSpeed() / v;
    releaseSpeedMagnitude_ = abs(speed_);
    carrier_->moving = true;
}

void ScrollView::carrierBounceStart()
{
    timerMode_ = TimerMode::Bouncing;
    timer_->start();
    startPos_ = carrier_->pos();
    finalPos_ = carrierStep(carrier_->pos());
    carrier_->moving = true;
}

void ScrollView::carrierTraverseStart(Step distance)
{
    timerMode_ = TimerMode::Traversing;
    timer_->start();
    startPos_ = carrier_->pos();
    finalPos_ = carrierStep(carrier_->pos() + distance);
    carrier_->moving = true;
}

void ScrollView::carrierStop()
{
    carrier_->moving = false;
    carrier_->pos = finalPos_;
    timerStop();
    carrierStopped();
}

void ScrollView::timerStop()
{
    timer_->stop();
    timerMode_ = TimerMode::Stopped;
}

static double fastAbs(Vector<double, 2> v)
{
    double a = 0;
    if (v[0] == 0) a = std::abs(v[1]);
    else if (v[1] == 0) a = std::abs(v[0]);
    else a = cc::abs(v);
    return a;
}

void ScrollView::carrierFly()
{
    double t = System::now();
    double T = t - lastTime_;
    lastTime_ = t;
    Step d = -speed_ * T;
    double va = fastAbs(speed_);
    double vb = va - (T / maxFlyTime()) * releaseSpeedMagnitude_;
    if (vb <= 0) vb = 0;
    speed_ *= vb / va;

    Point pa = carrier_->pos();
    Point pb = carrierStep(carrier_->pos() + d, boundary());
    if (pa == pb || speed_ == Step{}) {
        timerStop();
        if (carrierInsideBoundary()) {
            carrierBounceStart();
        }
        else {
            carrier_->moving = false;
            carrierStopped();
        }
    }
    else
        carrier_->pos = pb;
}

void ScrollView::carrierBounce()
{
    const double t0 = timer_->startTime();
    const double t1 = t0 + maxBounceTime();
    const double t = System::now();

    if (t >= t1) {
        carrierStop();
        return;
    }

    double s = easing::outBounce((t - t0) / (t1 - t0));
    carrier_->pos = (1 - s) * startPos_ + s * finalPos_;
}

void ScrollView::carrierTraverse()
{
    const double t0 = timer_->startTime();
    const double t1 = t0 + traverseTime();
    const double t = System::now();

    if (t >= t1) {
        carrierStop();
        return;
    }

    double s = easing::inQuad((t - t0) / (t1 - t0));
    carrier_->pos = (1 - s) * startPos_ + s * finalPos_;
}

void ScrollView::carrierStopped()
{
    preheat();
}

void ScrollView::keepFocusControlInView()
{
    if (!carrier_) return;
    if (!focusControl()) return;
    if (!carrier_->isParentOf(focusControl())) return;
    const InputField *inputField = nullptr;
    for (const View *view = focusControl()->parent(); view && !inputField; view = view->parent())
        inputField = Object::cast<const InputField *>(view);
    if (inputField) {
        if (inputField->isFullyVisibleIn(this)) return;
        Point topLeft = inputField->mapToParent(this, Point{});
        Point bottomRight = topLeft + inputField->size();
        if (!withinBounds(topLeft)) {
            Step step;
            if (topLeft[0] < 0) step[0] = -topLeft[0];
            if (topLeft[1] < 0) step[1] = -topLeft[1];
            if (step != Step{}) {
                carrierTraverseStart(step + Step{dp(10)});
                return;
            }
        }
        if (!withinBounds(bottomRight)) {
            Step step;
            if (bottomRight[0] > size()[0]) step[0] = size()[0] - bottomRight[0];
            if (bottomRight[1] > size()[1]) step[1] = size()[1] - bottomRight[1];
            if (step != Step{}) {
                carrierTraverseStart(step - Step{dp(10)});
                return;
            }
        }
    }
}

}} // namespace cc::ui
