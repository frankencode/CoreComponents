/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ListView>
#include <cc/Application>
#include <cc/Layout>

namespace cc {

class ListView::Pane final: public View
{
    friend class Object;
    friend class ListView;

    struct State final: public View::State
    {
        State()
        {
            itemVisibility([this]{
                if (!hasParent()) return;

                const double ls = leadSpace();

                if (height() <= parent().height()) {
                    for (auto stop: layout_()) {
                        View &child = stop.item();
                        child.pos(Point{0, ls + stop.pos()});
                        child.visible(true);
                    }
                }
                else {
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
                        const double yc = stop.pos();
                        if (yc + stop.extent() < y0 || y1 <= yc) {
                            const_cast<View &>(child).visible(false);
                        }
                        else {
                            ++i;
                        }
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

        void deplete()
        {
            if (layout_.count() == 0) return;

            layout_ = cc::Layout<View, double>{};
            View::State::deplete();
        }

        Property<void> itemVisibility;
        Property<double> layoutExtent;
        Property<double> leadSpace;
        Property<double> tailSpace;
        cc::Layout<View, double> layout_;
    };

    Pane():
        View{new State}
    {}

    State &me() { return View::me().as<State>(); }
    const State &me() const { return View::me().as<State>(); }

    State *operator->() { return &me(); }
};

ListView::State::State():
    Flickable::State{Pane{}}
{
    Pane carrier = Flickable::State::carrier().as<Pane>();

    carrier->leadSpace([this]{
        return header() ? header().height() : gu(2);
    });

    carrier->tailSpace([this]{
        return footer() ? footer().height() : gu(2);
    });
}

void ListView::State::deplete()
{
    carrier().as<Pane>()->deplete();
}

void ListView::State::setHeader(const View &newValue)
{
    if (header()) remove(header());
    header(newValue);
    View::State::insertChild(header());
    header().pos([this]{ return carrier().pos(); });
    header().visible([this]{ return header().height() >= -carrier().pos()[1]; });
}

void ListView::State::setFooter(const View &newValue)
{
    if (footer()) remove(footer());
    footer(newValue);
    View::State::insertChild(footer());
    footer().pos([this]{ return carrier().pos() + Point{0, carrier().height() - footer().height()}; });
    footer().visible([this]{ return footer().pos()[1] < height(); });
}

ListView::ListView():
    Flickable{onDemand<State>}
{}

ListView::ListView(double width, double height):
    Flickable{new State}
{
    size(width, height);
}

ListView &ListView::associate(Out<ListView> self)
{
    return View::associate<ListView>(self);
}

View ListView::header() const
{
    return me().header();
}

ListView &ListView::header(const View &newValue)
{
    me().setHeader(newValue);
    return *this;
}

View ListView::footer() const
{
    return me().footer();
}

ListView &ListView::footer(const View &newValue)
{
    me().setFooter(newValue);
    return *this;
}

void ListView::deplete()
{
    me().deplete();
}

ListView::State &ListView::me()
{
    return Object::me.as<State>();
}

const ListView::State &ListView::me() const
{
    return Object::me.as<State>();
}

} // namespace cc
