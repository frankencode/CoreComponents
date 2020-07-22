/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/SlideView>
#include <cc/ui/RowLayout>

namespace cc {
namespace ui {

SlideView::Instance::Instance()
{
    build >>[=]{
        if (parent()) {
            pos->restrict([](Point &, Point){ return false; });
            size <<[=]{ return parent()->size(); };
        }

        slideCarrier_ = View{};
        slideCarrier_->pos <<[=]{
            return currentIndex() * Point{ -size()[0], 0 };
        };
        (*this) << slideCarrier_;

        RowLayout{slideCarrier_};

        slideCount <<[=]{ return slideCarrier_->childCount(); };
        currentIndex->restrict([=](int &newIndex, int){
            if (newIndex < 0) newIndex = 0;
            if (newIndex >= slideCount()) newIndex = slideCount() - 1;
            return true;
        });
    };
}

View SlideView::Instance::currentSlide() const
{
    return slideCarrier_->childAt(currentIndex());
}

View SlideView::Instance::slideCarrier() const
{
    return slideCarrier_;
}

void SlideView::Instance::insertChild(View child)
{
    if (slideCarrier_) {
        child->size <<[=]{ return size(); };
        adoptChild(slideCarrier_, child);
    }
    else
        View::Instance::insertChild(child);
}

}} // namespace cc::ui
