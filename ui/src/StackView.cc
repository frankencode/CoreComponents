/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/StackView>

namespace cc {

struct StackView::State: public View::State
{
    State()
    {
        add(
            carrier_
            .pos([this]{
                return Point{-width() * (carrier_.count() - 1), 0.};
            })
            .size([this]{
                return Size{width() * carrier_.count(), height()};
            })
        );
    }

    void settled() override
    {
        carrier_.posEasing(Easing::Bezier{0.5, 0.0, 0.5, 1.0}, 0.2);
    }

    void push(const View &screen)
    {
        int screenIndex = stack_.count();
        stack_.pushBack(
            View{screen}
            .pos([this,screenIndex]{ return Point{screenIndex * width(), 0}; })
            .size([this]{ return size(); })
        );
        carrier_.add(screen);
    }

    View carrier_;
    List<View> stack_;
};

StackView::StackView():
    View{onDemand<State>}
{}

StackView::StackView(Out<StackView> self):
    View{new State}
{}

StackView &StackView::push(const View &screen)
{
    me().push(screen);
    return *this;
}

StackView &StackView::easing(const EasingCurve &easing, double duration)
{
    me().carrier_.posEasing(easing, duration);
    return *this;
}

void StackView::pop()
{

}

void StackView::home()
{}

StackView::State &StackView::me()
{
    return View::me().as<State>();
}

const StackView::State &StackView::me() const
{
    return View::me().as<State>();
}

} // namespace cc
