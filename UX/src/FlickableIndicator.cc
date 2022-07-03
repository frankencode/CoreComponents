/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/FlickableIndicator>
#include <cc/Flickable>
#include <cc/Box>

namespace cc {

struct FlickableIndicator::State final: public View::State
{
    State(Orientation initialOrientation = Orientation::Vertical):
        orientation{initialOrientation}
    {
        margin([this]{
            return style().flickableIndicatorHandleMargin();
        });

        thickness([this]{
            return style().flickableIndicatorThickness();
        });

        size([this]{
            return orientation() == Orientation::Vertical ?
                Size{
                    thickness(),
                    hasParent() ? parent().height() : 0
                } :
                Size{
                    hasParent() ? parent().width() : 0,
                    thickness()
                };
        });

        pos([this]{
            return orientation() == Orientation::Vertical ?
                Point{
                    hasParent() ? parent().width() - width() : 0,
                    0
                } :
                Point{
                    0,
                    hasParent() ? parent().height() - height() : 0
                };
        });

        paper([this]{
            return theme().flickableIndicatorColor();
        });

        visible([this]{
            return !autoHide() || visibleExtent() < totalExtent();
        });

        View handle;
        Box handleBar;

        add(
            View{}
            .associate(&handle)
            .size([this]{
                Size newSize;
                if (orientation() == Orientation::Vertical) {
                    const double w = width();
                    double h = height() * visibleExtent() / totalExtent();
                    if (h < minHandleSize()) h = minHandleSize();
                    if (h > height()) h = height();
                    newSize = Size{w, h};
                }
                else {
                    const double h = height();
                    double w = width() * visibleExtent() / totalExtent();
                    if (w < minHandleSize()) w = minHandleSize();
                    if (w > width()) w = width();
                    newSize = Size{w, h};
                }
                return newSize;
            })
            .add(
                Box{}
                .associate(&handleBar)
                .color([this]{
                    return theme().flickableIndicatorHandleColor();
                })
                .radius([this,handleBar]{
                    return (orientation() == Orientation::Vertical ? handleBar.width() : handleBar.height()) / 2;
                })
                .pos([this]{
                    return margin();
                })
                .size([this,handle,handleBar]{
                    return handle.size() - 2 * handleBar.pos();
                })
            )
        );

        visibleOffset([this]{
            if (!parent_()) return 0.;
            Flickable::State *flickableState = dynamic_cast<Flickable::State *>(parent_());
            if (!flickableState) return 0.;
            return
                orientation() == Orientation::Vertical ?
                flickableState->carrier_.pos()[1] :
                flickableState->carrier_.pos()[0];
        });

        totalExtent([this]{
            if (!parent_()) return 0.;
            Flickable::State *flickableState = dynamic_cast<Flickable::State *>(parent_());
            if (!flickableState) return 0.;
            return
                orientation() == Orientation::Vertical ?
                flickableState->carrier_.height() :
                flickableState->carrier_.width();
        });

        attach(Monitor{[this,handle]() mutable {
            if (orientation() == Orientation::Vertical) {
                const double h = handle.height();
                double y = (visibleOffset() < 0) * (-visibleOffset() / totalExtent()) * height();
                if (y + h > height()) y = height() - h;
                handle.pos(Point{0, y});
            }
            else {
                const double w = handle.width();
                double x = (visibleOffset() < 0) * (-visibleOffset() / totalExtent()) * width();
                if (x + w > width()) x = width() - w;
                handle.pos(Point{x, 0});
            }
        }});

    }

    double visibleExtent() const
    {
        return orientation() == Orientation::Vertical ? height() : width();
    }

    double minHandleSize() const { return 4 * thickness(); }

    Size preferredSize() const override { return minSize(); }

    Size minSize() const override
    {
        return orientation() == Orientation::Vertical ?
            Size{thickness(), minHandleSize()} :
            Size{minHandleSize(), thickness()};
    }

    Size maxSize() const override
    {
        return orientation() == Orientation::Vertical ?
            Size{thickness(), std::numeric_limits<double>::max()} :
            Size{std::numeric_limits<double>::max(), thickness()};
    }

    Property<Orientation> orientation { Orientation::Vertical };
    Property<bool> autoHide { true };
    Property<double> margin;
    Property<double> thickness;
    Property<double> visibleOffset;
    Property<double> totalExtent;
};

FlickableIndicator::FlickableIndicator():
    View{onDemand<State>}
{}

FlickableIndicator::FlickableIndicator(Orientation orientation):
    View{new State{orientation}}
{}

double FlickableIndicator::visibleOffset() const
{
    return me().visibleOffset();
}

FlickableIndicator &FlickableIndicator::visibleOffset(double newValue)
{
    me().visibleOffset(newValue);
    return *this;
}

FlickableIndicator &FlickableIndicator::visibleOffset(Definition<double> &&f)
{
    me().visibleOffset(move(f));
    return *this;
}

double FlickableIndicator::totalExtent() const
{
    return me().totalExtent();
}

FlickableIndicator &FlickableIndicator::totalExtent(double newValue)
{
    me().totalExtent(newValue);
    return *this;
}

FlickableIndicator &FlickableIndicator::totalExtent(Definition<double> &&f)
{
    me().totalExtent(move(f));
    return *this;
}

double FlickableIndicator::autoHide() const
{
    return me().autoHide();
}

FlickableIndicator &FlickableIndicator::autoHide(bool newValue)
{
    me().autoHide(newValue);
    return *this;
}

FlickableIndicator &FlickableIndicator::autoHide(Definition<bool> &&f)
{
    me().autoHide(move(f));
    return *this;
}

double FlickableIndicator::visibleExtent() const
{
    return me().visibleExtent();
}

FlickableIndicator::State &FlickableIndicator::me()
{
    return View::me().as<State>();
}

const FlickableIndicator::State &FlickableIndicator::me() const
{
    return View::me().as<State>();
}

} // namespace cc
