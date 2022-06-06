/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/DragArea>
#include <cc/Application>

namespace cc {

DragArea::State::State()
{
    dragZone([this]{
        return size();
    });

    onPointerPressed([this](const PointerEvent &event)
    {
        dragStart_ = event.pos();
        isDragged_ = false;
        dragTarget = findChild(event.pos());
        if (dragTarget()) {
            dragTarget().moveToTop();
            dragOrigin_ = dragTarget().pos();
        }
        return false;
    });

    onPointerReleased([this](const PointerEvent &event)
    {
        dragTarget = View{};
        return true;
    });

    onPointerMoved([this](const PointerEvent &event)
    {
        if (dragTarget()) {
            if (!isDragged_ && dragStart(event, dragStart_)) {
                isDragged_ = true;
            }
            if (isDragged_) {
                Rect zone = dragZone();
                Step delta = dragDistance(event, dragStart_);
                if (dragTarget().width() == zone.width()) delta[0] = 0;
                else if (dragTarget().height() == zone.height()) delta[1] = 0;
                Rect next{dragOrigin_ + delta, dragTarget().size()};
                if (!zone.contains(next)) {
                    if (next.height() < zone.height()) {
                        double yMin = zone.y();
                        double yMax = zone.y() + zone.height();
                        double y0 = next.y();
                        double y1 = next.y() + next.height();
                        if (y0 < yMin) next.pos(next.x(), yMin);
                        else if (yMax < y1) next.pos(next.x(), yMax - next.height());
                    }

                    if (next.width() < zone.width()) {
                        double xMin = zone.x();
                        double xMax = zone.x() + zone.width();
                        double x0 = next.x();
                        double x1 = next.x() + next.width();
                        if (x0 < xMin) next.pos(xMin, next.y());
                        else if (xMax < x1) next.pos(xMax - next.width(), next.y());
                    }
                }
                if (zone.contains(next))
                    drag(next.pos());
            }
        }

        return isDragged_;
    });
}

bool DragArea::State::dragStart(const PointerEvent &event, Point dragStart) const
{
    return !dragInhibit() && Application{}.pointerIsDragged(event, dragStart);
}

Step DragArea::State::dragDistance(const PointerEvent &event, Point dragStart) const
{
    return event.pos() - dragStart;
}

void DragArea::State::drag(Point newPos)
{
    dragTarget().pos(newPos);
}

DragArea::DragArea():
    Control{onDemand<DragArea::State>}
{}

DragArea::DragArea(double width, double height):
    Control{new State}
{
    size(Size{width, height});
}

DragArea &DragArea::associate(Out<DragArea> self)
{
    return View::associate<DragArea>(self);
}

} // namespace cc
