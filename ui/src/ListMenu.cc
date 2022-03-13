/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/ListMenu>

namespace cc::ui {

class ListMenu::Carrier: public View
{
    friend class Object;
    friend class ListMenu;

    struct State: public View::State
    {
        State()
        {
            itemVisibility([this]{
                if (!hasParent()) return;

                const long ni = childrenCount();
                const double wi = parent().width();
                const double hi = style().itemHeight2();
                const double ls = leadSpace();
                const double ts = tailSpace();
                const double hc = hi * ni + ls + ts;
                const Size itemSize{wi, hi};

                if (hc <= parent().height()) {
                    double y = ls;
                    for (Locator pos = children_.head(); pos; ++pos) {
                        children_.modifyAt(pos, [=](View &child){
                            child.pos(Point{0, y});
                            child.size(itemSize);
                            child.visible(true);
                        });
                        y += hi;
                    }
                }
                else {
                    const double y0 = -pos().y();
                    const double y1 = y0 + parent().height();
                    long i0 = std::floor((y0 - ls) / hi);
                    long i1 = std::ceil ((y1 - ls) / hi);
                    if (i0 < 0) i0 = 0;
                    if (ni < i1) i1 = ni;
                    double y = i0 * hi + ls;
                    for (Locator pos = children_.from(i0); pos && +pos < i1; ++pos) {
                        children_.modifyAt(pos, [=](View &child){
                            child.pos(Point{0, y});
                            child.size(itemSize);
                            child.visible(true);
                        });
                        y += hi;
                    }
                    for (long i = 0; i < visibleChildren_.count();) {
                        const View &child = visibleChildren_.at(i);
                        if (child.y() + hi < y0 || y1 <= child.y())
                            visibleChildren_.modifyAt(i, [](View &child){
                                child.visible(false);
                            });
                        else
                            ++i;
                    }
                }
            });

            size([this]{
                if (!hasParent()) return Size{};
                const long ni = childrenCount();
                const double wi = parent().width();
                const double hi = style().itemHeight2();
                const double ls = leadSpace();
                const double ts = tailSpace();
                return Size{wi, hi * ni + ls + ts};
            });
        }

        bool isPainted() const override { return false; }

        Property<void> itemVisibility;
        Property<void> itemHighlighting;
        Property<double> leadSpace;
        Property<double> tailSpace;
    };

    Carrier():
        View{new State}
    {}

    State &me() { return View::me().as<State>(); }
    const State &me() const { return View::me().as<State>(); }
};

ListMenu::State::State():
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

void ListMenu::State::insertChild(View child)
{
    child.visible(false);
    Flickable::State::insertChild(child);
}

ListMenu::ListMenu():
    Flickable{onDemand<State>}
{}

ListMenu &ListMenu::header(const View &newValue)
{
    if (me().header()) remove(me().header());
    me().header(newValue);
    me().View::State::insertChild(me().header());
    me().header().pos([this]{ return me().carrier().pos(); });
    return *this;
}

ListMenu &ListMenu::footer(const View &newValue)
{
    if (me().footer()) remove(me().footer());
    me().footer(newValue);
    me().View::State::insertChild(me().footer());
    me().footer().pos([this]{ return me().carrier().pos() + Point{0, me().carrier().height() - me().footer().height()}; });
    return *this;
}

} // namespace cc::ui
