/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/debug>
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
    isDragged_(false)
{
    carrier_ = View::create(this);

    if (parent)
        size->bind([=]{ return parent->size(); });
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
    return false;
}

bool ScrollView::onPointerReleased(const PointerEvent *event)
{
    isDragged_ = false;
    return false;
}

bool ScrollView::onPointerMoved(const PointerEvent *event)
{
    if (
        !isDragged_ &&
        (event->pos() - dragStart_)->magnitudeSquared() >=
        minimumDragDistance() * minimumDragDistance()
    ) {
        isDragged_ = true;
        carrierOrigin_ = carrier_->pos();
    }

    if (isDragged_) {
        Step d = event->pos() - dragStart_;
        Point p = carrierOrigin_ + d;

        double x = p[0];
        double y = p[1];
        double w = carrier_->size()[0];
        double h = carrier_->size()[1];

        if (x > 0 || (w < size()[0])) x = 0;
        else if (x + w < size()[0]) x = size()[0] - w;

        if (y > 0 || (h < size()[1])) y = 0;
        else if (y + h < size()[1]) y = size()[1] - h;

        carrier_->pos = Point{ x, y };
    }

    return true;
}

}} // namespace cc::ui
