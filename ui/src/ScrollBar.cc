/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ScrollBar>
#include <cc/Box>
#include <cc/Application>
#include <cc/Easing>

namespace cc {

struct ScrollBar::State: public DragArea::State
{
    State(Orientation initialOrientation = Orientation::Vertical):
        orientation{initialOrientation}
    {
        margin([this]{
            return style().scrollHandleMargin();
        });

        thickness([this]{
            return style().scrollBarThickness();
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
            return theme().inputFieldFillColor();
        });

        needToShow([this]{
            if (!autoHide()) return true;
            if (isHandheld()) return false;

            return visibleExtent() < totalExtent() && (
                parent().isParentOf(Application{}.hoverControl()) ||
                parent().isParentOf(Application{}.pressedControl())
            );
        });

        View handle;
        handle.size([this]{
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
        });

        handleHover([=,this]{
            return hover() && handle.containsGlobal(mapToGlobal(pointerPos()));
        });

        handlePressed([=,this]{
            return dragTarget() == handle;
        });

        {
            Box handleBar;
            handleBar.color([this]{
                if (handlePressed()) return theme().focusInputLineColor();
                if (handleHover()) return theme().secondaryTextColor();
                return theme().inactiveTextColor();
            });
            handleBar.pos([this]{
                return margin();
            });
            handleBar.size([=,this]{
                return handle.size() - 2 * handleBar.pos();
            });
            handleBar.radius([=,this]{
                return (orientation() == Orientation::Vertical ? handleBar.width() : handleBar.height()) / 2;
            });
            handle.add(handleBar);
        }
        add(handle);

        hideLater_.onTimeout([this]{
            opacity = 0.;
            hideLater_.stop();
        });

        opacity(needToShow() * 1.);
        Easing{opacity, Easing::Linear, style().scrollBarFadeOutDuration()};

        visible([this]{ return opacity() > 0; });

        monitorCarrier([=,this]() mutable {
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
        });

        monitorNeedToShow([this]() mutable {
            hideLater_.stop();
            if (needToShow()) opacity(1.);
            else hideLater_.start();
        });
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

    bool dragStart(const PointerEvent &event, Point dragStart) const override
    {
        return true;
    }

    Step dragDistance(const PointerEvent &event, Point dragStart) const override
    {
        return orientation() == Orientation::Vertical ?
            Step{0, (event.pos() - dragStart).y()} :
            Step{(event.pos() - dragStart).x(), 0};
    }

    void drag(Point newPos) override
    {
        double p = // relative position [0..1]
            orientation() == Orientation::Vertical ?
            newPos.y() / (height() - dragTarget().height()) :
            newPos.x() / (width()  - dragTarget().width() );

        visibleOffset(p * (visibleExtent() - totalExtent()));
    }

    Property<Orientation> orientation { Orientation::Vertical };
    Property<bool> handleHover;
    Property<bool> handlePressed;
    Property<bool> autoHide;
    Property<bool> needToShow;
    Property<double> margin;
    Property<double> thickness;
    Property<double> visibleOffset;
    Property<double> totalExtent;
    Property<void> monitorCarrier;
    Property<void> monitorNeedToShow;

    Timer hideLater_ { style().scrollBarAutoHideTimeout() };
};

ScrollBar::ScrollBar():
    DragArea{onDemand<State>}
{}

ScrollBar::ScrollBar(Out<ScrollBar> self):
    DragArea{new State{Orientation::Vertical}}
{
    self = weak<ScrollBar>();
}

ScrollBar::ScrollBar(Orientation orientation, Out<ScrollBar> self):
    DragArea{new State{orientation}}
{
    self = weak<ScrollBar>();
}

Orientation ScrollBar::orientation() const
{
    return me().orientation();
}

ScrollBar &ScrollBar::orientation(Orientation newValue)
{
    me().orientation(newValue);
    return *this;
}

double ScrollBar::visibleOffset() const
{
    return me().visibleOffset();
}

ScrollBar &ScrollBar::visibleOffset(double newValue)
{
    me().visibleOffset(newValue);
    return *this;
}

ScrollBar &ScrollBar::visibleOffset(Definition<double> &&f)
{
    me().visibleOffset(std::move(f));
    return *this;
}

double ScrollBar::totalExtent() const
{
    return me().totalExtent();
}

ScrollBar &ScrollBar::totalExtent(double newValue)
{
    me().totalExtent(newValue);
    return *this;
}

ScrollBar &ScrollBar::totalExtent(Definition<double> &&f)
{
    me().totalExtent(std::move(f));
    return *this;
}

double ScrollBar::autoHide() const
{
    return me().autoHide();
}

ScrollBar &ScrollBar::autoHide(bool newValue)
{
    me().autoHide(newValue);
    return *this;
}

ScrollBar &ScrollBar::autoHide(Definition<bool> &&f)
{
    me().autoHide(std::move(f));
    return *this;
}

double ScrollBar::visibleExtent() const
{
    return me().visibleExtent();
}

ScrollBar::State &ScrollBar::me()
{
    return View::me().as<State>();
}

const ScrollBar::State &ScrollBar::me() const
{
    return View::me().as<State>();
}

} // namespace cc
