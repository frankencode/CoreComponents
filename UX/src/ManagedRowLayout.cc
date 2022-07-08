/*
 * Copyright (C) 2020-2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/ManagedRowLayout>
#include <cc/Window>

namespace cc {

struct ManagedRowLayout::State final: public View::Layout::State
{
    State() = default;

    bool isManaged() const override { return true; }
    Orientation orientation() const override { return Orientation::Horizontal; }

    void updateMinSize();
    void updateMaxSize();
    void updatePreferredSize();
    void updateChildHeights();
    void updateChildWidths(Size neededSize);
    void updateLayout();

    Property<RowAlign> align { RowAlign::Top };
    Property<void> update { [this]{ updateLayout(); } };
    bool firstTime_ { true };
    bool setViewSize_ { false };
    bool updating_ { false };
};

void ManagedRowLayout::State::updateMinSize()
{
    double minWidth = 2 * margin()[0] + spacing() * (view().visibleChildren().count() - (view().visibleChildren().count() > 0));
    double minHeight = 0.;

    for (const View &child: view().children())
    {
        if (!child.visible()) continue;

        const double h = child.minSize()[1] + child.padding()[1];
        if (h > minHeight) minHeight = h;

        minWidth += child.padding()[0] + child.minSize()[0];
    }

    if (minHeight != std::numeric_limits<double>::max()) {
        minHeight += 2 * margin()[1];
    }

    minSize(Size{minWidth, minHeight});
}

void ManagedRowLayout::State::updateMaxSize()
{
    double maxWidth = 2 * margin()[0] + spacing() * (view().visibleChildren().count() - (view().visibleChildren().count() > 0));
    double maxHeight = 0;

    for (const View &child: view().children())
    {
        if (!child.visible()) continue;

        if (maxWidth < std::numeric_limits<double>::max()) {
            if (child.maxSize()[0] == std::numeric_limits<double>::max()) {
                maxWidth = std::numeric_limits<double>::max();
            }
            else {
                maxWidth += child.padding()[0] + child.maxSize()[0];
            }
        }

        if (maxHeight < child.maxSize()[1]) maxHeight = child.maxSize()[1];
    }

    if (maxHeight != std::numeric_limits<double>::max()) {
        maxHeight += 2 * margin()[1];
    }

    maxSize(Size{maxWidth, maxHeight});
}

void ManagedRowLayout::State::updatePreferredSize()
{
    double width = 2 * margin()[0] + spacing() * (view().visibleChildren().count() - (view().visibleChildren().count() > 0));
    double height = 0;

    for (const View &child: view().children())
    {
        if (!child.visible()) continue;

        width += child.padding()[0] + (0 < child.preferredSize()[0] ? child.preferredSize()[0] : child.size()[0]);

        if (height < child.maxSize()[1]) height = child.maxSize()[1];
    }

    height += 2 * margin()[1];

    preferredSize(Size{width, height});
}

void ManagedRowLayout::State::updateChildHeights()
{
    double availHeight = view().height() - 2 * margin()[1];

    for (const View &child: view().children())
    {
        if (!child.visible()) continue;

        if (child.size()[1] < availHeight) {
            // vertically expand child view
            View target = child;
            if (availHeight <= child.preferredSize()[1]) target.size(child.size()[0], availHeight);
            else target.size(child.size()[0], child.preferredSize()[1]);
        }
        else if (availHeight < child.size()[0]) {
            // vertically shrink child view
            View target = child;
            if (child.minSize()[1] < availHeight) target.size(child.size()[0], availHeight);
            else target.size(child.size()[0], child.minSize()[1]);
            // take care of eventually changed minimum width (e.g. when a text unwraps)
            if (child.size()[0] < child.minSize()[0]) target.size(child.minSize()[0], child.size()[1]);
        }
    }
}

void ManagedRowLayout::State::updateChildWidths(Size neededSize)
{
    if (neededSize[0] < view().size()[0])
    {
        /** Grow expansible children
          */
        for (int t = 0; t < 10; ++t)
        {
            bool tryAgain = false;
            int expansibleCount = 0;
            for (const View &child: view().children()) {
                if (!child.visible()) continue;
                if (child.size()[0] < child.maxSize()[0]) {
                    ++expansibleCount;
                }
            }
            if (expansibleCount > 0) {
                double extraWidth = view().size()[0] - neededSize[0];
                double delta = extraWidth / expansibleCount;
                for (const View &child: view().children()) {
                    if (!child.visible()) continue;
                    if (child.size()[0] < child.maxSize()[0]) {
                        View target = child;
                        double newWidth = child.size()[0] + delta;
                        if (newWidth > child.maxSize()[0]) {
                            target.size(child.maxSize()[0], child.size()[1]);
                            tryAgain = true;
                            break;
                        }
                        target.size(newWidth, child.size()[1]);
                    }
                }
            }
            if (!tryAgain) break;
        }
    }
    else if (view().size()[0] < neededSize[0])
    {
        /** Reduce shrinkable children
          */
        for (int t = 0; t < 10; ++t)
        {
            bool tryAgain = false;
            int shrinkableCount = 0;
            for (const View &child: view().children()) {
                if (!child.visible()) continue;
                if (child.minSize()[0] < child.size()[0]) {
                    ++shrinkableCount;
                }
            }
            if (shrinkableCount > 0) {
                double extraWidth = neededSize[0] - view().size()[0];
                double delta = extraWidth / shrinkableCount;
                for (const View &child: view().children()) {
                    if (!child.visible()) continue;
                    if (child.minSize()[0] < child.size()[0]) {
                        View target = child;
                        double newWidth = child.size()[0] - delta;
                        if (newWidth < child.minSize()[0]) {
                            target.size(child.minSize()[0], child.size()[1]);
                            tryAgain = true;
                            break;
                        }
                        target.size(newWidth, child.size()[1]);
                    }
                }
            }
            if (!tryAgain) break;
        }
    }
}

void ManagedRowLayout::State::updateLayout()
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

    if (view().size() != Size{}) updateChildHeights();

    updateMinSize();
    updateMaxSize();
    updatePreferredSize();

    Size innerSize;
    {
        for (const View &child: view().children())
        {
            if (!child.visible()) continue;

            const double h = child.size()[1] + child.padding()[1];
            if (h > innerSize[1]) innerSize[1] = h;

            innerSize[0] += child.padding()[0] + child.size()[0];
        }

        innerSize[0] += spacing() * (view().visibleChildren().count() - (view().visibleChildren().count() > 0));
    }

    Size neededSize = innerSize + 2 * margin();

    if (firstTime_) {
        firstTime_ = false;
        setViewSize_ = (view().size() == Size{});
    }

    if (setViewSize_) {
        setViewSize_ = false;
        view().size = neededSize;
    }
    else {
        updateChildWidths(neededSize);
    }

    double x = 0;

    for (View child: view().children())
    {
        if (!child.visible()) continue;

        x += child.padding().top();

        if (align() == RowAlign::Top)
            child.pos(Point{x, child.padding().top()} + margin());
        else if (align() == RowAlign::Bottom)
            child.pos(Point{x, innerSize[1] - child.size()[1] - child.padding().bottom()} + margin());
        else
            child.pos(Point{x, 0.5 * (innerSize[1] - child.size()[1])} + margin());

        x += child.size()[0];
        x += child.padding().bottom();
        x += spacing();
    }
}

ManagedRowLayout::ManagedRowLayout():
    Layout{new ManagedRowLayout::State}
{}

RowAlign ManagedRowLayout::align() const
{
    return me().align();
}

ManagedRowLayout &ManagedRowLayout::align(RowAlign newValue)
{
    me().align(newValue);
    return *this;
}

ManagedRowLayout::State& ManagedRowLayout::me()
{
    return Object::me.as<State>();
}

const ManagedRowLayout::State& ManagedRowLayout::me() const
{
    return Object::me.as<State>();
}

} // na
