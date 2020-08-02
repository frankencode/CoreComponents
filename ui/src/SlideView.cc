/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/SlideView>
#include <cc/ui/RowLayout>
#include <cc/ui/Easing>

namespace cc {
namespace ui {

SlideView::Instance::Instance()
{
    size <<[=]{ return parent() ? parent()->size() : Size{}; };

    slideCarrier_ = View{};
    slideCarrier_->pos <<[=]{
        return currentIndex() * Point{ -size()[0], 0 };
    };
    Easing(slideCarrier_->pos, 0.5, Easing::Bezier(0.5, -0.4, 0.5, 1.4));
    RowLayout{slideCarrier_};
    View::Instance::insertChild(slideCarrier_);

    slideCount <<[=]{ return slideCarrier_->childCount(); };
    currentIndex->restrict([=](int &newIndex, int){
        if (newIndex < 0) newIndex = 0;
        if (newIndex >= slideCount()) newIndex = slideCount() - 1;
        return true;
    });

    keyPressed >>[=](const KeyEvent *event)
    {
        if (event->scanCode() == ScanCode::Key_Left)
        {
            currentIndex -= 1;
        }
        else if (event->scanCode() == ScanCode::Key_Right)
        {
            currentIndex += 1;
        }
        else if ('0' <= +event->keyCode() && +event->keyCode() <= '9')
        {
            currentIndex = +event->keyCode() - '1' + 10 * (+event->keyCode() == '0');
        }
        return true;
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
    child->size <<[=]{ return size(); };
    adoptChild(slideCarrier_, child);
}

}} // namespace cc::ui
