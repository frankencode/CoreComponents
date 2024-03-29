/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/ColumnLayout>
#include <cc/Window>

namespace cc {

struct ColumnLayout::State final: public View::Layout::State
{
    State() = default;

    Orientation orientation() const override { return Orientation::Vertical; }

    void updateLayout();

    Property<ColumnAlign> align { ColumnAlign::Left };
    Property<double> indent { 0 };

    Property<void> update { [this]{ updateLayout(); } };
    bool firstTime_ { true };
    bool setViewSize_ { false };
};

void ColumnLayout::State::updateLayout()
{
    if (!hasView() || !view().hasWindow()) return;

    view().childrenCount();

    Size innerSize;
    {
        for (const View &child: view().children())
        {
            if (!child.visible()) continue;

            const double padddedWidth = child.size()[0] + child.padding()[0];
            if (padddedWidth > innerSize[0]) innerSize[0] = padddedWidth;
            innerSize[1] += child.padding()[1] + child.size()[1];
        }
        innerSize[1] += spacing() * (view().visibleChildren().count() - (view().visibleChildren().count() > 0));
    }

    if (firstTime_) { /// \todo move this outside the binding
        firstTime_ = false;
        setViewSize_ = (view().size() == Size{});
    }
    Size viewSize = setViewSize_ ? innerSize + Size{indent(), 0} + 2 * margin() : view().size();
    if (setViewSize_) {
        double preferredWidth = view().preferredSize()[0];
        if (preferredWidth > 0) viewSize[0] = preferredWidth;
        view().size(viewSize);
    }

    double y = 0;

    for (View child: view().children())
    {
        if (!child.visible()) continue;

        y += child.padding().top();

        if (align() == ColumnAlign::Auto || align() == ColumnAlign::Left)
            child.pos(Point{indent() + child.padding().left(), y} + margin());
        else if (align() == ColumnAlign::Right)
            child.pos(Point{innerSize[0] - child.size()[0] - child.padding().right(), y} + margin());
        else
            child.pos(Point{0.5 * (innerSize[0] - child.size()[0]), y} + margin());

        y += child.size()[1];
        y += child.padding().bottom();
        y += spacing();
    }
}

ColumnLayout::ColumnLayout():
    Layout{new ColumnLayout::State}
{}

ColumnAlign ColumnLayout::align() const
{
    return me().align();
}

ColumnLayout &ColumnLayout::align(ColumnAlign newValue)
{
    me().align(newValue);
    return *this;
}

double ColumnLayout::indent() const
{
    return me().indent();
}

ColumnLayout &ColumnLayout::indent(double newValue)
{
    me().indent(newValue);
    return *this;
}

ColumnLayout::State& ColumnLayout::me()
{
    return Object::me.as<State>();
}

const ColumnLayout::State& ColumnLayout::me() const
{
    return Object::me.as<State>();
}

} // namespace cc
