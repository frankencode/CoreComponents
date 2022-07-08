/*
 * Copyright (C) 2020-2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/ManagedColumnLayout>
#include <cc/Window>

namespace cc {

struct ManagedColumnLayout::State final: public View::Layout::State
{
    State() = default;

    bool isManaged() const override { return true; }
    Orientation orientation() const override { return Orientation::Vertical; }

    void updateMinSize();
    void updateMaxSize();
    void updatePreferredSize();
    void updateChildWidths();
    void updateChildHeights(Size neededSize);
    void updateLayout();

    Property<ColumnAlign> align { ColumnAlign::Left };
    Property<double> indent { 0 };
    Property<void> update { [this]{ updateLayout(); } };
    bool firstTime_ { true };
    bool setViewSize_ { false };
    bool updating_ { false };
};

void ManagedColumnLayout::State::updateMinSize()
{
    double minWidth = 0.;
    double minHeight = 2 * margin()[1] + spacing() * (view().visibleChildren().count() - (view().visibleChildren().count() > 0));

    for (const View &child: view().children())
    {
        if (!child.visible()) continue;

        const double w = child.minSize()[0] + child.padding()[0];
        if (w > minWidth) minWidth = w;

        minHeight += child.padding()[1] + child.minSize()[1];
    }

    if (minWidth != std::numeric_limits<double>::max()) {
        minWidth += 2 * margin()[0] + indent();
    }

    minSize(Size{minWidth, minHeight});
}

void ManagedColumnLayout::State::updateMaxSize()
{
    double maxWidth = 0.;
    double maxHeight = 2 * margin()[1] + spacing() * (view().visibleChildren().count() - (view().visibleChildren().count() > 0));

    for (const View &child: view().children())
    {
        if (!child.visible()) continue;

        if (maxHeight < std::numeric_limits<double>::max()) {
            if (child.maxSize()[1] == std::numeric_limits<double>::max()) {
                maxHeight = std::numeric_limits<double>::max();
            }
            else {
                maxHeight += child.padding()[1] + child.maxSize()[1];
            }
        }

        if (maxWidth < child.maxSize()[0]) maxWidth = child.maxSize()[0];
    }

    if (maxWidth != std::numeric_limits<double>::max()) {
        maxWidth += 2 * margin()[0] + indent();
    }

    maxSize(Size{maxWidth, maxHeight});
}

void ManagedColumnLayout::State::updatePreferredSize()
{
    double width = 0.;
    double height = 2 * margin()[1] + spacing() * (view().visibleChildren().count() - (view().visibleChildren().count() > 0));

    for (const View &child: view().children())
    {
        if (!child.visible()) continue;

        height += child.padding()[1] + (0 < child.preferredSize()[1] ? child.preferredSize()[1] : child.size()[1]);

        if (width < child.preferredSize()[0]) width = child.preferredSize()[0];
    }

    width += 2 * margin()[0] + indent();

    preferredSize(Size{width, height});
}

void ManagedColumnLayout::State::updateChildWidths()
{
    double availWidth = view().width() - 2 * margin()[0] - indent();

    for (const View &child: view().children())
    {
        if (!child.visible()) continue;

        if (child.size()[0] < availWidth) {
            // horizonally expand child view
            View target = child;
            if (availWidth <= child.preferredSize()[0]) target.size(availWidth, child.size()[1]);
            else target.size(child.preferredSize()[0], child.size()[1]);
        }
        else if (availWidth < child.size()[0]) {
            // horizontally shrink child view
            View target = child;
            if (child.minSize()[0] < availWidth) target.size(availWidth, child.size()[1]);
            else target.size(child.minSize()[0], child.size()[1]);
            // take care of eventually change minimum height (e.g. when a text wraps)
            if (child.size()[1] < child.minSize()[1]) target.size(child.size()[0], child.minSize()[1]);
        }
    }
}

void ManagedColumnLayout::State::updateChildHeights(Size neededSize)
{
    if (neededSize[1] < view().size()[1])
    {
        /** Grow expansible children
          */
        for (int t = 0; t < 10; ++t)
        {
            bool tryAgain = false;
            int expansibleCount = 0;
            for (const View &child: view().children()) {
                if (!child.visible()) continue;
                if (child.size()[1] < child.maxSize()[1]) {
                    ++expansibleCount;
                }
            }
            if (expansibleCount > 0) {
                double extraHeight = view().size()[1] - neededSize[1];
                double delta = extraHeight / expansibleCount;
                for (const View &child: view().children()) {
                    if (!child.visible()) continue;
                    if (child.size()[1] < child.maxSize()[1]) {
                        View target = child;
                        double newHeight = child.size()[1] + delta;
                        if (newHeight > child.maxSize()[1]) {
                            target.size(child.size()[0], child.maxSize()[1]);
                            tryAgain = true;
                            break;
                        }
                        target.size(child.size()[0], newHeight);
                    }
                }
            }
            if (!tryAgain) break;
        }
    }
    else if (view().size()[1] < neededSize[1])
    {
        /** Reduce shrinkable children
          */
        for (int t = 0; t < 10; ++t)
        {
            bool tryAgain = false;
            int shrinkableCount = 0;
            for (const View &child: view().children()) {
                if (!child.visible()) continue;
                if (child.minSize()[1] < child.size()[1]) {
                    ++shrinkableCount;
                }
            }
            if (shrinkableCount > 0) {
                double extraHeight = neededSize[1] - view().size()[1];
                double delta = extraHeight / shrinkableCount;
                for (const View &child: view().children()) {
                    if (!child.visible()) continue;
                    if (child.minSize()[1] < child.size()[1]) {
                        View target = child;
                        double newHeight = child.size()[1] - delta;
                        if (newHeight < child.minSize()[1]) {
                            target.size(child.size()[0], child.minSize()[1]);
                            tryAgain = true;
                            break;
                        }
                        target.size(child.size()[0], newHeight);
                    }
                }
            }
            if (!tryAgain) break;
        }
    }
}

void ManagedColumnLayout::State::updateLayout()
{
    if (!hasView() || !view().window() || updating_) return;

    class Guard {
    public:
        Guard(bool *flag): flag_{flag} { *flag_ = true; }
        ~Guard() { *flag_ = false; }
    private:
        bool *flag_;
    };

    Guard guard{&updating_};

    view().childrenCount();

    if (view().size() != Size{}) updateChildWidths();

    updateMinSize();
    updateMaxSize();
    updatePreferredSize();

    Size innerSize;
    {
        for (const View &child: view().children())
        {
            if (!child.visible()) continue;

            const double w = child.size()[0] + child.padding()[0];
            if (w > innerSize[0]) innerSize[0] = w;

            innerSize[1] += child.padding()[1] + child.size()[1];
        }
        innerSize[1] += spacing() * (view().visibleChildren().count() - (view().visibleChildren().count() > 0));
    }

    Size neededSize = innerSize + Size{indent(), 0} + 2 * margin();

    if (firstTime_) {
        firstTime_ = false;
        setViewSize_ = (view().size() == Size{});
    }

    if (setViewSize_) {
        setViewSize_ = false;
        view().size(neededSize);
    }
    else {
        updateChildHeights(neededSize);
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

ManagedColumnLayout::ManagedColumnLayout():
    Layout{new ManagedColumnLayout::State}
{}

ColumnAlign ManagedColumnLayout::align() const
{
    return me().align();
}

ManagedColumnLayout &ManagedColumnLayout::align(ColumnAlign newValue)
{
    me().align(newValue);
    return *this;
}

double ManagedColumnLayout::indent() const
{
    return me().indent();
}

ManagedColumnLayout &ManagedColumnLayout::indent(double newValue)
{
    me().indent(newValue);
    return *this;
}

ManagedColumnLayout::State& ManagedColumnLayout::me()
{
    return Object::me.as<State>();
}

const ManagedColumnLayout::State& ManagedColumnLayout::me() const
{
    return Object::me.as<State>();
}

} // namespace cc
