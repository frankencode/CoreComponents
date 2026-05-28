/*
 * Copyright (C) 2026 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/GridLayout>
#include <cc/Window>

namespace cc {

struct GridLayout::State final: public View::Layout::State
{
    State() = default;

    Orientation orientation() const override { return Orientation::Vertical; }

    void updateLayout();

    Property<int> columns { 2 };
    Property<RowAlign> rowAlign { RowAlign::Center };
    Property<ColumnAlign> columnAlign { ColumnAlign::Left };

    Property<void> update { [this]{ updateLayout(); } };
    bool firstTime_ { true };
    bool setViewSize_ { false };
};

void GridLayout::State::updateLayout()
{
    if (!hasView() || !view().hasWindow()) return;

    int childrenCount = view().childrenCount();
    int colCount = columns();
    int rowCount = childrenCount / colCount + (childrenCount % colCount > 0);

    Array<double> colExtent { Dim<>{ colCount } };
    Array<double> rowExtent { Dim<>{ rowCount } };
    colExtent.fill(0);
    rowExtent.fill(0);
    {
        int row = 0, col = 0;

        for (const View &child: view().children())
        {
            if (!child.visible()) continue;

            Padding padding = child.padding();
            Size childSize = child.size() + Size { padding.left() + padding.right(), padding.top() + padding.bottom() };

            if (colExtent[col] < childSize[0]) colExtent[col] = childSize[0];
            if (rowExtent[row] < childSize[1]) rowExtent[row] = childSize[1];

            if (++col == colCount) {
                col = 0;
                ++row;
            }
        }
    }

    {
        Size innerSize;

        for (double extent: colExtent) {
            innerSize[0] += extent;
        }
        for (double extent: rowExtent) {
            innerSize[1] += extent;
        }

        innerSize += Size {
            spacing() * (colCount - 1),
            spacing() * (rowCount - 1)
        };

        if (firstTime_) {
            firstTime_ = false;
            setViewSize_ = (view().size() == Size{});
        }
        if (setViewSize_) {
            view().size = innerSize + 2 * margin();
        }
    }

    int row = 0, col = 0;
    double x = 0, y = 0;

    for (View child: view().children())
    {
        if (!child.visible()) continue;

        Rect cell {
            margin() + Point {
                x + child.padding().left(),
                y + child.padding().top()
            },
            Size {
                colExtent[col] - child.padding().left() - child.padding().right(),
                rowExtent[row] - child.padding().top() - child.padding().bottom()
            }
        };

        double x0 = 0;
        double y0 = 0;

        switch (columnAlign()) {
            case ColumnAlign::Left: {
                x0 = cell.x0();
                break;
            }
            case ColumnAlign::Right: {
                x0 = cell.x1() - child.size()[0];
                break;
            }
            case ColumnAlign::Center: {
                x0 = (cell.x0() + cell.x1() - child.size()[0]) / 2;
                break;
            }
            case ColumnAlign::Auto: {
                x0 = cell.x0();
                break;
            }
        };

        switch (rowAlign()) {
            case RowAlign::Center: {
                y0 = (cell.y0() + cell.y1() - child.size()[1]) / 2;
                break;
            }
            case RowAlign::Top: {
                y0 = cell.y0();
                break;
            }
            case RowAlign::Bottom: {
                y0 = cell.y1() - child.size()[1];
                break;
            }
        };

        child.pos({x0, y0});

        x += colExtent[col];
        x += spacing();

        if (++col == colCount) {
            x = 0;
            y += rowExtent[row];
            y += spacing();
            col = 0;
            ++row;
        }
    }
}

GridLayout::GridLayout():
    Layout{new State}
{}

int GridLayout::columns() const
{
    return me().columns();
}

GridLayout &GridLayout::columns(int newValue)
{
    me().columns(newValue);
    return *this;
}

GridLayout &GridLayout::columns(Definition<int> &&f)
{
    me().columns(std::move(f));
    return *this;
}

RowAlign GridLayout::rowAlign() const
{
    return me().rowAlign();
}

GridLayout &GridLayout::rowAlign(RowAlign newValue)
{
    me().rowAlign(newValue);
    return *this;
}

GridLayout &GridLayout::rowAlign(Definition<RowAlign> &&f)
{
    me().rowAlign(std::move(f));
    return *this;
}

ColumnAlign GridLayout::columnAlign() const
{
    return me().columnAlign();
}

GridLayout &GridLayout::columnAlign(ColumnAlign newValue)
{
    me().columnAlign(newValue);
    return *this;
}

GridLayout &GridLayout::columnAlign(Definition<ColumnAlign> &&f)
{
    me().columnAlign(std::move(f));
    return *this;
}

GridLayout::State& GridLayout::me()
{
    return Object::me.as<State>();
}

const GridLayout::State& GridLayout::me() const
{
    return Object::me.as<State>();
}

} // namespace cc
