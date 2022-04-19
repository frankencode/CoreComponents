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
    friend class Object;
    friend class ListView;

    struct State final: public View::State
    {
        State()
        {
            itemVisibility([this]{
                if (!hasParent()) return;

                if (height() <= parent().height()) {
                    for (auto stop: layout_()) {
                        View &child = stop.item();
                        child.pos(Point{0, stop.pos()});
                        child.visible(true);
                    }
                }
                else {
                    const double ls = leadSpace();
                    const double y0 = -y() - ls;
                    const double y1 = y0 + parent().height();

                    for (auto stop: layout_(y0, y1)) {
                        View &child = stop.item();
                        child.pos(Point{0, ls + stop.pos()});
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
                        layoutExtent(layout_.extent());
                    });
                }
                return Size{parent().width(), leadSpace() + layoutExtent() + tailSpace()};
            });
        }

        void insertChild(View child) override
        {
            child.pos(Point{0, leadSpace() + layout_.extent()});
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
        Property<double> leadSpace;
        Property<double> tailSpace;
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
    {
        Carrier carrier = Flickable::State::carrier().as<Carrier>();

        carrier.me().leadSpace([this]{
            return header() ? header().height() : gu(2);
        });

        carrier.me().tailSpace([this]{
            return footer() ? footer().height() : gu(2);
        });
    }

    Property<View> header;
    Property<View> footer;
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

View ListView::header() const
{
    return me().header();
}

ListView &ListView::header(const View &newValue)
{
    if (me().header()) remove(me().header());
    me().header(newValue);
    me().View::State::insertChild(me().header());
    me().header().pos([this]{ return carrier().pos(); });
    me().header().visible([this]{ return header().height() >= -carrier().pos()[1]; });
    return *this;
}

View ListView::footer() const
{
    return me().footer();
}

ListView &ListView::footer(const View &newValue)
{
    if (me().footer()) remove(me().footer());
    me().footer(newValue);
    me().View::State::insertChild(me().footer());
    me().footer().pos([this]{ return carrier().pos() + Point{0, carrier().height() - footer().height()}; });
    me().footer().visible([this]{ return footer().pos()[1] < height(); });
    return *this;
}

ListView::State &ListView::me()
{
    return Object::me.as<State>();
}

const ListView::State &ListView::me() const
{
    return Object::me.as<State>();
}

} // namespace cc::ui
