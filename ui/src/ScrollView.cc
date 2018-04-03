/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/debug>
#include <cc/System>
#include <cc/ui/Timer>
#include <cc/ui/ScrollView>

namespace cc {
namespace ui {

Ref<ScrollView> ScrollView::create(View *parent)
{
    return Object::create<ScrollView>(parent);
}

ScrollView::ScrollView(View *parent):
    View(parent),
    carrier_(0),
    isDragged_(false),
    timer_(Timer::create(1./60))
{
    carrier_ = View::create(this);

    if (parent) size->bind([=]{ return parent->size(); });

    timer_->triggered->connect([=]{ animate(); });
}

void ScrollView::insertChild(View *child)
{
    if (carrier_)
        adoptChild(carrier_, child);
    else
        View::insertChild(child);
}

bool ScrollView::hasPointerInput() const
{
    return true;
}

bool ScrollView::onPointerPressed(const PointerEvent *event)
{
    dragStart_ = event->pos();
    isDragged_ = false;
    speed_ = Point{};
    if (timer_->isActive()) timer_->stop();
    return false;
}

bool ScrollView::onPointerReleased(const PointerEvent *event)
{
    isDragged_ = false;
    if (speed_ != Step{}) {
        timer_->start();
        lastTime_ = timer_->startTime();
        double v = abs(speed_);
        if (v > maxSpeed()) speed_ *= maxSpeed() / v;
        // CC_INSPECT(fixed(speed_, 5, 3));
        releaseSpeedMagnitude_ = abs(speed_);
    }
    return false;
}

bool ScrollView::onPointerMoved(const PointerEvent *event)
{
    if (
        !isDragged_ &&
        absPow2(event->pos() - dragStart_) >= minDragDistance() * minDragDistance()
    ) {
        isDragged_ = true;
        carrierOrigin_ = carrier_->pos();
        lastDragTime_ = 0;
    }

    if (isDragged_) {
        double t = System::now();
        if (lastDragTime_ > 0)
            speed_ = (lastDragPos_ - event->pos()) / (t - lastDragTime_);
        lastDragTime_ = t;
        lastDragPos_ = event->pos();

        Step d = event->pos() - dragStart_;
        Point p = carrierOrigin_ + d;

        carrier_->pos = carrierStep(p);
    }

    return true;
}

Point ScrollView::carrierStep(Point p)
{
    double x = p[0];
    double y = p[1];
    double w = carrier_->size()[0];
    double h = carrier_->size()[1];

    if (x > 0 || (w < size()[0])) x = 0;
    else if (x + w < size()[0]) x = size()[0] - w;

    if (y > 0 || (h < size()[1])) y = 0;
    else if (y + h < size()[1]) y = size()[1] - h;

    return Point{ x, y };
}

void ScrollView::animate()
{
    double t = System::now();
    double T = t - lastTime_;
    lastTime_ = t;
    Step d = -speed_ * T;
    double va = 0; {
        if (speed_[0] == 0) va = std::abs(speed_[1]);
        else if (speed_[1] == 0) va = std::abs(speed_[0]);
        else va = cc::abs(speed_);
    }
    double vb = va - (T / maxFlyTime()) * releaseSpeedMagnitude_;
    if (vb <= 0) vb = 0;
    speed_ *= vb / va;

    Point pa = carrier_->pos();
    Point pb = carrierStep(carrier_->pos() + d);
    if (pa == pb || speed_ == Step{})
        timer_->stop();
    else
        carrier_->pos = pb;
}

}} // namespace cc::ui
