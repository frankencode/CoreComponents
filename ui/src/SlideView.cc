/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/RowLayout>
#include <cc/ui/SlideView>

namespace cc {
namespace ui {

Ref<SlideView> SlideView::create(View *parent)
{
    return Object::create<SlideView>(parent);
}

SlideView::SlideView(View *parent):
    View(parent)
{
    if (parent) {
        pos->restrict([](Point &, Point){ return false; });
        size->bind([=]{ return parent->size(); });
    }

    slideCarrier_ = View::create(this);
    slideCarrier_->pos->bind([=]{ return currentIndex() * Point{ -size()[0], 0 }; });
    RowLayout::setup(slideCarrier_);

    slideCount->bind([=]{ return slideCarrier_->childCount(); });
    currentIndex->restrict([=](int &newIndex, int){
        if (newIndex < 0) newIndex = 0;
        if (newIndex >= slideCount()) newIndex = slideCount() - 1;
        return true;
    });
}

View *SlideView::currentSlide() const
{
    return slideCarrier_->childAt(currentIndex());
}

View *SlideView::slideCarrier() const
{
    return slideCarrier_;
}

void SlideView::insertChild(View *child)
{
    if (slideCarrier_) {
        child->size->bind([=]{ return size(); });
        adoptChild(slideCarrier_, child);
    }
    else
        View::insertChild(child);
}

}} // namespace cc::ui
