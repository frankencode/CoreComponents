/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ScrollView>

namespace cc {

ScrollView::State::State(const View &pane):
    flickable_{pane}
{
    vScroll_.orientation(Orientation::Vertical);
    hScroll_.orientation(Orientation::Horizontal);

    vScroll_.autoHide([this]{ return scrollBarAutoHide(); });
    hScroll_.autoHide([this]{ return scrollBarAutoHide(); });

    vScroll_.size([this]{
        return Size{
            vScroll_.minSize()[0],
            height() - hScroll_.visible() * hScroll_.height()
        };
    });

    hScroll_.size([this]{
        return Size{
            width() - vScroll_.visible() * vScroll_.width(),
            hScroll_.minSize()[1]
        };
    });

    vScroll_.totalExtent([this]{ return flickable_.pane().height(); });
    hScroll_.totalExtent([this]{ return flickable_.pane().width(); });

    flickable_.size([this]{
        return Size{
            width() - vScroll_.visible() * vScroll_.width(),
            height() - hScroll_.visible() * hScroll_.height()
        };
    });

    flickable_.clip(true);

    scrollMonitor([this]{
        flickable_.pane().pos(
            Point{
                hScroll_.visibleOffset(),
                vScroll_.visibleOffset()
            }
        );
    });

    carrierMonitor([this]{
        Point offset = flickable_.pane().pos();
        hScroll_.visibleOffset(offset.x());
        vScroll_.visibleOffset(offset.y());
    });

    add(flickable_);
    add(vScroll_);
    add(hScroll_);
}

ScrollView::ScrollView():
    View{onDemand<State>}
{}

ScrollView::ScrollView(double width, double height):
    View{new State}
{
    size(Size{width, height});
}

ScrollView::ScrollView(const View &pane):
    View{new State{pane}}
{}

ScrollView::ScrollView(double width, double height, const View &pane):
    View{new State{pane}}
{
    size(Size{width, height});
}

ScrollView &ScrollView::associate(Out<ScrollView> self)
{
    return View::associate<ScrollView>(self);
}

} // namespace cc
