/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ListMenu>

namespace cc {

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
                        children_.at(pos)
                            .pos(Point{0, y})
                            .size(itemSize)
                            .visible(true);
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
                        children_.at(pos)
                            .pos(Point{0, y})
                            .size(itemSize)
                            .visible(true);
                        y += hi;
                    }
                    for (long i = 0; i < visibleChildren_.count();) {
                        View &child = visibleChildren_.at(i);
                        if (child.y() + hi < y0 || y1 <= child.y())
                            child.visible(false);
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

    State *operator->() { return &me(); }
};

struct ListMenu::State final: public Flickable::State
{
    State():
        Flickable::State{Pane{}}
    {
        Pane carrier = Flickable::State::carrier().as<Pane>();

        carrier->leadSpace([this]{
            return header() ? header().height() : leadSpace();
        });

        carrier->tailSpace([this]{
            return footer() ? footer().height() : tailSpace();
        });
    }

    void setHeader(const View &newValue)
    {
        if (header()) remove(header());
        header(newValue);
        View::State::insertChild(header());
        header().pos([this]{ return carrier().pos(); });
        header().visible([this]{ return header().height() >= -carrier().pos()[1]; });
    }

    void setFooter(const View &newValue)
    {
        if (footer()) remove(footer());
        footer(newValue);
        View::State::insertChild(footer());
        footer().pos([this]{ return carrier().pos() + Point{0, carrier().height() - footer().height()}; });
        footer().visible([this]{ return footer().pos()[1] < height(); });
    }

    Property<View> header;
    Property<View> footer;
    Property<double> leadSpace { sp(8) };
    Property<double> tailSpace { sp(8) };
};

ListMenu::ListMenu():
    Flickable{onDemand<State>}
{}

ListMenu::ListMenu(double width, double height):
    Flickable{new State}
{
    size(width, height);
}

ListMenu &ListMenu::associate(Out<ListMenu> self)
{
    return View::associate<ListMenu>(self);
}

View ListMenu::header() const
{
    return me().header();
}

ListMenu &ListMenu::header(const View &newValue)
{
    me().setHeader(newValue);
    return *this;
}

View ListMenu::footer() const
{
    return me().footer();
}

ListMenu &ListMenu::footer(const View &newValue)
{
    me().setFooter(newValue);
    return *this;
}

double ListMenu::leadSpace() const
{
    return me().leadSpace();
}

ListMenu &ListMenu::leadSpace(double newValue)
{
    me().leadSpace(newValue);
    return *this;
}

double ListMenu::tailSpace() const
{
    return me().tailSpace();
}

ListMenu &ListMenu::tailSpace(double newValue)
{
    me().tailSpace(newValue);
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

} // namespace cc
