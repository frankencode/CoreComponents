/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/StackView>
#include <cc/RenderView>

namespace cc {

struct StackView::State: public View::State
{
    State(Size initialSize = Size{})
    {
        size(initialSize);

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

        attach([this]{
            size();
            postmortem_.visible(false);
        });
    }

    int screensCount() const { return carrier_.count() - 1/*postmortem_*/; }

    void settled() override
    {
        carrier_.posEasing(Easing::Bezier{0.5, 0.0, 0.5, 1.0}, 0.2);
    }

    void push(View screen)
    {
        int screenIndex = stack_.count();
        carrier_.add(
            screen
            .pos([this,screenIndex]{ return Point{screenIndex * width(), 0}; })
            .size([this]{ return size(); })
        );
        stack_.pushBack(screen);
    }

    void pop()
    {
        const View &topView = stack_.last();
        stack_.popBack();
        if (!postmortem_.visible()) postmortem_.visible(true);
        topView.renderTo(postmortem_.image());
        postmortem_.update();
        carrier_.remove(topView);
    }

    void home()
    {
        while (stack_.count() > 1) {
            carrier_.remove(stack_.last());
            stack_.popBack();
        }
    }

    View carrier_;
    RenderView postmortem_;
    List<View> stack_;
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
    me().home();
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
