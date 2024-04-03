/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/RowLayout>
#include <cc/Window>

namespace cc {

struct  RowLayout::State final: public View::Layout::State
{
    State() = default;

    Orientation orientation() const override { return Orientation::Horizontal; }

    void updateLayout();

    Property<RowAlign> align { RowAlign::Top };

    Property<void> update { [this]{ updateLayout(); } };
    bool firstTime_ { true };
    bool setViewSize_ { false };
};

void RowLayout::State::updateLayout()
{
    if (!hasView() || !view().hasWindow()) return;

    view().childrenCount();

    Size innerSize;
    {
        for (const View &child: view().children())
        {
            if (!child.visible()) continue;
            const double paddedHeight = child.size()[1] + child.padding()[1];
            if (paddedHeight > innerSize[1]) innerSize[1] = paddedHeight;
            innerSize[0] += child.padding()[0] + child.size()[0];
        }

        innerSize[0] += spacing() * (view().visibleChildren().count() - (view().visibleChildren().count() > 0));
    }

    if (firstTime_) {
        firstTime_ = false;
        setViewSize_ = (view().size() == Size{});
    }
    if (setViewSize_)
        view().size = innerSize + 2 * margin();

    double x = 0;

    for (View child: view().children())
    {
        if (!child.visible()) continue;

        x += child.padding().left();

        if (align() == RowAlign::Top)
            child.pos(Point{x, child.padding().top()} + margin());
        else if (align() == RowAlign::Bottom)
            child.pos(Point{x, innerSize[1] - child.size()[1] - child.padding().bottom()} + margin());
        else
            child.pos(Point{x, 0.5 * (innerSize[1] - child.size()[1])} + margin());

        x += child.size()[0];
        x += child.padding().right();
        x += spacing();
    }
}

RowLayout::RowLayout():
    Layout{new State}
{}

RowAlign RowLayout::align() const
{
    return me().align();
}

RowLayout &RowLayout::align(RowAlign newValue)
{
    me().align(newValue);
    return *this;
}

RowLayout::State& RowLayout::me()
{
    return Object::me.as<State>();
}

const RowLayout::State& RowLayout::me() const
{
    return Object::me.as<State>();
}

} // namespace cc
