/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/StackViewState>
#include <cc/Application>

namespace cc {

StackView::State::State(Size initialSize)
{
    size(initialSize);

    carrier_.touch();

    add(
        carrier_
        .pos([this]{
            return Point{-width() * (screensCount() - 1), 0.};
        })
        .size([this]{
            return Size{width() * (screensCount() + 1), height()};
        })
        .add(
            postmortem_
            .pos([this]{
                return Point{width() * screensCount(), 0.};
            })
            .size([this]{
                return size();
            })
        )
    );

    size.onChanged([this]{ postmortem_.visible(false); });
}

int StackView::State::screensCount() const
{
    return carrier_.childrenCount() - 1/*postmortem_*/;
}

void StackView::State::settled()
{
    carrier_.posEasing(
        Easing::Bezier{0.5, 0.0, 0.5, 1.0},
        0.3,
        [this]{ return sizing() || homing(); }
    );
}

void StackView::State::push(View screen)
{
    int screenIndex = screensCount();
    carrier_.add(
        screen
        .pos([this,screenIndex]{ return Point{screenIndex * width(), 0}; })
        .size([this]{ return size(); })
    );
    stack_.pushBack(screen);
}

void StackView::State::pop()
{
    View topView = stack_.last();
    if (!postmortem_.visible()) postmortem_.visible(true);
    postmortem_.image().clear(basePaper());
    topView.renderTo(postmortem_.image());
    postmortem_.update();
    stack_.popBack();
    Application{}.postEvent([carrier=carrier_, topView]() mutable {
        carrier.remove(topView);
    });
}

void StackView::State::home()
{
    homing(true);
    while (stack_.count() > 1) {
        carrier_.remove(stack_.last());
        stack_.popBack();
    }
    homing(false);
}

StackView::StackView():
    View{onDemand<State>}
{}

StackView::StackView(double width, double height):
    View{new State{Size{width, height}}}
{}

StackView::StackView(CreateState onDemand):
    View{onDemand}
{}

StackView::StackView(State *newState):
    View{newState}
{}

StackView &StackView::associate(Out<StackView> self)
{
    return View::associate<StackView>(self);
}

StackView &StackView::push(const View &view)
{
    me().push(view);
    return *this;
}

StackView &StackView::easing(const EasingCurve &easing, double duration)
{
    me().carrier_.posEasing(easing, duration);
    return *this;
}

void StackView::pop()
{
    me().pop();
}

void StackView::home()
{
    Application{}.postEvent([s=*this]() mutable {
        s.home();
    });
}

int StackView::count() const
{
    return me().carrier_.childrenCount() - 1;
}

const View &StackView::carrier() const
{
    return me().carrier_;
}

StackView::State &StackView::me()
{
    return View::me().as<State>();
}

const StackView::State &StackView::me() const
{
    return View::me().as<State>();
}

} // namespace cc
