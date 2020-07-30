/*
 * Copyright (C) 2018-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/ScrollView>
#include <cc/ui/Application>
#include <cc/ui/InputField>
#include <cc/ui/Easing>
#include <cc/ui/Timer>
#include <cc/System>
#include <cc/math>

namespace cc {
namespace ui {

ScrollView::Instance::Instance(const View &carrier):
    carrier_{carrier}
{
    size <<[=]{ return parent() ? parent()->size() : Size{}; };

    size >>[=]{
        positionCarrierOnResize();
        keepFocusControlInView();
    };

    timer_->timeout >>[=]{
        if (timerMode_ == TimerMode::Flying) carrierFly();
        else if (timerMode_ == TimerMode::Bouncing) carrierBounce();
        else if (timerMode_ == TimerMode::Traversing) carrierTraverse();
    };

    focusControl <<[=]{
        return Application{}->focusControl();
    };

    focusControl >>[=]{
        keepFocusControlInView();
    };

    if (!carrier_) carrier_ = View{};
    if (!carrier_->parent()) View::Instance::insertChild(carrier_);
}

ScrollView::Instance::~Instance()
{}

View ScrollView::Instance::createCarrier()
{
    return View{};
}

void ScrollView::Instance::preheat()
{}

void ScrollView::Instance::positionCarrierOnResize()
{
    carrier_->pos = carrierStep(carrier_->pos());
}

void ScrollView::Instance::resetCarrier()
{
    timerStop();
    removeChild(carrier_);
    carrier_ = nullptr;
    carrier_ = createCarrier();
    (*this) << carrier_;
}

void ScrollView::Instance::insertChild(View child)
{
    if (carrier_)
        adoptChild(carrier_, child);
    else
        View::Instance::insertChild(child);
}

View ScrollView::Instance::setLayout(const Layout &layout)
{
    return carrier_->setLayout(layout);
}

bool ScrollView::Instance::onPointerClicked(const PointerEvent *event)
{
    if (!carrierAtRest_) return true;

    Control control = getControlAt(event->pos());
    if (control) {
        PointerEvent::PosGuard guard{const_cast<PointerEvent *>(event), mapToChild(control, event->pos())};
        return control->onPointerClicked(event);
    }

    return true;
}

bool ScrollView::Instance::onPointerPressed(const PointerEvent *event)
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
        Control control = carrier()->getControlAt(mapToChild(carrier(), event->pos()));
        if (control) {
            Application{}->pressedControl = control;
            PointerEvent::PosGuard guard{const_cast<PointerEvent *>(event), mapToChild(control, event->pos())};
            return control->onPointerPressed(event);
        }
    }

    return true;
}

bool ScrollView::Instance::onPointerReleased(const PointerEvent *event)
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

bool ScrollView::Instance::onPointerMoved(const PointerEvent *event)
{
    if (!isDragged_ && Application{}->pointerIsDragged(event, dragStart_)) {
        isDragged_ = true;
        carrierOrigin_ = carrier_->pos();
        lastDragTime_ = 0;
        Application{}->pressedControl = Control{nullptr};
    }

    if (isDragged_) {
        double t = event->time();
        if (lastDragTime_ > 0 && t != lastDragTime_)
            speed_ = (lastDragPos_ - event->pos()) / (t - lastDragTime_);

        lastDragTime_ = t;
        lastDragPos_ = event->pos();

        Step d = event->pos() - dragStart_;
        Point p = carrierOrigin_ + d;

        carrier_->pos = carrierStep(p, boundary());
    }

    return true;
}

bool ScrollView::Instance::onWheelMoved(const WheelEvent *event)
{
    if (Application{}->focusControl())
        Application{}->focusControl = Control{nullptr};

    if (timerMode_ == TimerMode::Bouncing || timerMode_ == TimerMode::Traversing)
        carrierStop();

    if (timerMode_ != TimerMode::Stopped)
        return true;

    carrier_->pos = carrierStep(carrier_->pos() + event->wheelStep() * wheelGranularity(), wheelBouncing() ? boundary() : 0);

    if (carrierInsideBoundary())
        carrierBounceStart();

    return true;
}

bool ScrollView::Instance::onWindowLeft()
{
    if (carrierInsideBoundary())
        carrierBounceStart();

    return false;
}

bool ScrollView::Instance::carrierInsideBoundary() const
{
    double x = carrier_->pos()[0];
    double y = carrier_->pos()[1];
    double w = carrier_->size()[0];
    double h = carrier_->size()[1];

    return
        (w > size()[0] && (x > 0 || x + w < size()[0])) ||
        (h > size()[1] && (y > 0 || y + h < size()[1]));
}

Point ScrollView::Instance::carrierStep(Point p, double b)
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

    return Point{x, y};
}

void ScrollView::Instance::carrierFlyStart()
{
    timerMode_ = TimerMode::Flying;
    timer_->start();
    lastTime_ = timer_->startTime();
    double v = abs(speed_);
    if (v > maxSpeed()) speed_ *= maxSpeed() / v;
    releaseSpeedMagnitude_ = abs(speed_);
    carrier_->moving = true;
}

void ScrollView::Instance::carrierBounceStart()
{
    timerMode_ = TimerMode::Bouncing;
    timer_->start();
    startPos_ = carrier_->pos();
    finalPos_ = carrierStep(carrier_->pos());
    carrier_->moving = true;
}

void ScrollView::Instance::carrierTraverseStart(Step distance)
{
    timerMode_ = TimerMode::Traversing;
    timer_->start();
    startPos_ = carrier_->pos();
    finalPos_ = carrierStep(carrier_->pos() + distance);
    carrier_->moving = true;
}

void ScrollView::Instance::carrierStop()
{
    carrier_->moving = false;
    carrier_->pos = finalPos_;
    timerStop();
    carrierStopped();
}

void ScrollView::Instance::timerStop()
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

void ScrollView::Instance::carrierFly()
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

void ScrollView::Instance::carrierBounce()
{
    const double t0 = timer_->startTime();
    const double t1 = t0 + maxBounceTime();
    const double t = System::now();

    if (t >= t1) {
        carrierStop();
        return;
    }

    double s = Easing::outBounce((t - t0) / (t1 - t0));
    carrier_->pos = (1 - s) * startPos_ + s * finalPos_;
}

void ScrollView::Instance::carrierTraverse()
{
    const double t0 = timer_->startTime();
    const double t1 = t0 + traverseTime();
    const double t = System::now();

    if (t >= t1) {
        carrierStop();
        return;
    }

    double s = Easing::inQuad((t - t0) / (t1 - t0));
    carrier_->pos = (1 - s) * startPos_ + s * finalPos_;
}

void ScrollView::Instance::carrierStopped()
{
    preheat();
}

void ScrollView::Instance::keepFocusControlInView()
{
    if (!carrier_) return;
    if (!focusControl()) return;
    if (!carrier_->isParentOf(focusControl())) return;
    InputField inputField = nullptr;
    for (View view = focusControl()->parent(); view && !inputField; view = view->parent())
        inputField = view->as<InputField>();
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
