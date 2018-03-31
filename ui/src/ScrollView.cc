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
    PointerInput(this),
    carrier_(0),
    isDragged_(false)
{
    carrier_ = View::create(this);

    if (parent)
        size->bind([=]{ return parent->size(); });

    pressed->connect([=]{
        dragStart_ = mousePos();
    });

    released->connect([=]{
        isDragged_ = false;
    });

    hovered->connect([=]{
        if (
            !isDragged_ &&
            mouseButton() == MouseButton::Left &&
            (mousePos() - dragStart_)->magnitudeSquared() >=
            minimumDragDistance() * minimumDragDistance()
        ) {
            isDragged_ = true;
            carrierOrigin_ = carrier_->pos();
        }

        if (isDragged_) dragged();
    });
}

void ScrollView::insertChild(View *child)
{
    if (carrier_)
        adoptChild(carrier_, child);
    else
        View::insertChild(child);
}

void ScrollView::dragged()
{
    Step d = mousePos() - dragStart_;
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

}} // namespace cc::ui
