/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/ListMenu>

namespace cc::ui {

class ListMenu::Pane final: public View
{
    friend class Object;
    friend class ListMenu;

    struct State final: public View::State
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
                    const double y0 = -y();
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

        void insertChild(View child) override
        {
            child.visible(false);
            View::State::insertChild(child);
        }

        Property<void> itemVisibility;
        Property<void> itemHighlighting;
        Property<double> leadSpace;
        Property<double> tailSpace;
    };

    Pane():
        View{new State}
    {}

    State &me() { return View::me().as<State>(); }
    const State &me() const { return View::me().as<State>(); }
};

struct ListMenu::State final: public Flickable::State
{
    State():
        Flickable::State{Pane{}}
    {
        Pane pane = Flickable::State::pane().as<Pane>();

        pane.me().leadSpace([this]{
            return header() ? header().height() : gu(2);
        });

        pane.me().tailSpace([this]{
            return footer() ? footer().height() : gu(2);
        });
    }

    Property<View> header;
    Property<View> footer;
};

ListMenu::ListMenu():
    Flickable{onDemand<State>}
{}

ListMenu::ListMenu(Out<ListMenu> self):
    Flickable{new State}
{
    self = weak<ListMenu>();
}

ListMenu::ListMenu(double width, double height):
    Flickable{new State}
{
    size(width, height);
}

View ListMenu::header() const
{
    return me().header();
}

ListMenu &ListMenu::header(const View &newValue)
{
    if (me().header()) remove(me().header());
    me().header(newValue);
    me().View::State::insertChild(me().header());
    me().header().pos([this]{ return pane().pos(); });
    me().header().visible([this]{ return header().height() >= -pane().pos()[1]; });
    return *this;
}

View ListMenu::footer() const
{
    return me().footer();
}

ListMenu &ListMenu::footer(const View &newValue)
{
    if (me().footer()) remove(me().footer());
    me().footer(newValue);
    me().View::State::insertChild(me().footer());
    me().footer().pos([this]{ return pane().pos() + Point{0, pane().height() - footer().height()}; });
    me().footer().visible([this]{ return footer().pos()[1] < height(); });
    return *this;
}

ListMenu::State &ListMenu::me()
{
    return View::me().as<State>();
}

const ListMenu::State &ListMenu::me() const
{
    return View::me().as<State>();
}

} // namespace cc::ui
