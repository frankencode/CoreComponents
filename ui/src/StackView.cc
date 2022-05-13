/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/StackView>
#include <cc/RenderView>
#include <cc/Application>

namespace cc {

struct StackView::State final: public View::State
{
    State(Size initialSize = Size{})
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

    int screensCount() const { return carrier_.childrenCount() - 1/*postmortem_*/; }

    void settled() override
    {
        carrier_.posEasing(
            Easing::Bezier{0.5, 0.0, 0.5, 1.0},
            0.3,
            [this]{ return sizing() || homing(); }
        );
    }

    void push(View screen)
    {
        int screenIndex = screensCount();
        carrier_.add(
            screen
            .pos([this,screenIndex]{ return Point{screenIndex * width(), 0}; })
            .size([this]{ return size(); })
        );
        stack_.pushBack(screen);
    }

    void pop()
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

    void home()
    {
        homing(true);
        while (stack_.count() > 1) {
            carrier_.remove(stack_.last());
            stack_.popBack();
        }
        homing(false);
    }

    View carrier_;
    RenderView postmortem_;
    List<View> stack_;
    Property<bool> homing { false };
};

StackView::StackView():
    View{onDemand<State>}
{}

StackView::StackView(Out<StackView> self):
    View{new State}
{
    self = *this;
}

StackView::StackView(double width, double height, Out<StackView> self):
    View{new State{Size{width, height}}}
{
    self = *this;
}

StackView &StackView::push(View screen)
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
