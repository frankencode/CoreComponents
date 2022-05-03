/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/StackView>
#include <cc/RenderView>
#include <cc/DEBUG>

namespace cc {

struct StackView::State: public View::State
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
            /*.attach([this]{
                CC_INSPECT(carrier_.size());
                CC_INSPECT(carrier_.pos());
                CC_INSPECT(screensCount());
            })*/
        );

        attach([this]{
            size();
            postmortem_.visible(false);
        });
    }

    int screensCount() const { return carrier_.childrenCount() - 1/*postmortem_*/; }

    void settled() override
    {
        carrier_.posEasing(Easing::Linear, 1);
    }

    void push(View screen)
    {
        int screenIndex = screensCount();
        carrier_.add(
            screen
            .pos([this,screenIndex]{ return Point{screenIndex * width(), 0}; })
            .size([this]{ return size(); })
        );
        CC_INSPECT(screen.fullId());
        stack_.pushBack(screen);
        // screen.update();
        // screen.expose();
        //screen.visible(false);
        // screen.visible(true);
    }

    void pop()
    {
        const View &topView = stack_.last();
        if (!postmortem_.visible()) postmortem_.visible(true);
        // postmortem_.image().clear(topView.paper());
        postmortem_.image().clear(basePaper());
        topView.renderTo(postmortem_.image());
        // postmortem_.image().premultiply();
        postmortem_.update();
        CC_INSPECT(carrier_.childrenCount());
        carrier_.remove(topView);
        CC_INSPECT(carrier_.childrenCount());
        stack_.popBack();
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
