/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/ListView>
#include <cc/ui/Application>
#include <cc/Layout>

namespace cc::ui {

class ListView::Carrier final: public View
{
    friend class ListView;

    struct State final: public View::State
    {
        State()
        {
            itemVisibility([this]{
                if (!hasParent()) return;
                if (children_.count() == 0) return;

                if (height() <= parent().height()) {
                    for (auto stop: layout_()) {
                        View &child = stop.item();
                        child.pos(Point{0, stop.pos()});
                        child.visible(true);
                    }
                }
                else {
                    const double y0 = -y();
                    const double y1 = y0 + parent().height();

                    for (auto stop: layout_(y0, y1)) {
                        View &child = stop.item();
                        child.pos(Point{0, stop.pos()});
                        child.visible(true);
                    }
                    for (long i = 0; i < visibleChildren_.count();) {
                        const View &child = visibleChildren_.at(i);
                        auto stop = layout_.at(layout_.indexOf(child));
                        if (child.y() + stop.extent() < y0 || y1 <= stop.pos())
                            visibleChildren_.modifyAt(i, [](View &child){
                                child.visible(false);
                            });
                        else
                            ++i;
                    }
                }
            });

            size([this]{
                if (layout_.count() > 0) {
                    Application{}.postEvent([this]{
                        for (long i = 0; i < layout_.count(); ++i) {
                            auto stop = layout_.at(i);
                            const View &view = stop.item();
                            if (view.height() != stop.extent()) {
                                stop.setExtent(view.height());
                            }
                        }
                        // TODO... logic for smartly moving the carrier (keep view at end or first item in view, etc.)
                        layoutExtent(layout_.extent());
                    });
                }
                return Size{parent().width(), layoutExtent()};
            });
        }

        void insertChild(View child) override
        {
            child.pos(Point{0, layout_.extent()});
            child.size(Size{width(), child.height()});
            View::State::insertChild(child);
            layout_.pushBack(child, child.height());
            layoutExtent(layout_.extent());
        }

        void removeChild(View child) override
        {
            layout_.remove(child);
            View::State::removeChild(child);
            layoutExtent(layout_.extent());
        }

        Property<void> itemVisibility;
        Property<double> layoutExtent;
        cc::Layout<View, double> layout_;
    };

    Carrier():
        View{new State}
    {}

    State &me() { return View::me().as<State>(); }
    const State &me() const { return View::me().as<State>(); }
};

struct ListView::State final: public Flickable::State
{
    State():
        Flickable::State{Carrier{}}
    {}
};

ListView::ListView():
    Flickable{onDemand<State>}
{}

ListView::ListView(Out<ListView> self):
    Flickable{new State}
{
    self = *this;
}

ListView::ListView(double width, double height):
    Flickable{new State}
{
    size(width, height);
}

} // namespace cc::ui
