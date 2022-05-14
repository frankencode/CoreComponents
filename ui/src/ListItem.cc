/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ListItem>
#include <cc/ImageView>
#include <cc/Label>
#include <cmath>

namespace cc {

ListItem::State::State()
{
    size([this]{
        return parent() ? Size{parent().width(), style().itemHeight2()} : minSize();
    });

    paper([this]{
        return pressed() ? theme().itemHighlightColor() : Color{};
    });
}

Size ListItem::State::minSize() const
{
    return Size{
        sp(16) +
        (icon() ? icon().width() + sp(16) : 0) +
        (title() ? title().minSize()[0] + sp(16) : 0) +
        (indicator() ? indicator().width() + sp(16) : 0),
        style().itemHeight2()
    };
}

void ListItem::State::bindIcon()
{
    icon().pos([this]{
        return Point{
            sp(16),
            (height() - icon().height()) / 2
        };
    });
}

void ListItem::State::bindTitle()
{
    title().pos([this]{
        return Point{
            (icon() ? icon().right() : 0) + sp(16),
            std::round((height() - title().height()) / 2)
        };
    });

    title().size([this]{
        Size minItemSize = minSize();
        if (minItemSize[0] <= size()[0]) return title().minSize();
        return title().minSize() - Size{minItemSize[0] - size()[0], 0};
    });
}

void ListItem::State::bindIndicator()
{
    indicator().pos([this]{
        return Point{
            width() - indicator().width() - sp(16),
            std::round((height() - indicator().height()) / 2)
        };
    });
}

ListItem::ListItem():
    Control{onDemand<State>}
{}

ListItem &ListItem::associate(Out<ListItem> self)
{
    return View::associate<ListItem>(self);
}

ListItem &ListItem::icon(const View &newValue)
{
    if (me().icon()) remove(me().icon());
    add(me().icon = newValue);
    me().bindIcon();
    return *this;
}

ListItem &ListItem::iconImage(const Image &newValue)
{
    return icon(ImageView{newValue});
}

ListItem &ListItem::title(const View &newValue)
{
    if (me().title()) remove(me().title());
    add(me().title = newValue);
    me().bindTitle();
    return *this;
}

ListItem &ListItem::text(const String &newValue)
{
    return title(Label{newValue});
}

ListItem &ListItem::indicator(const View &newValue)
{
    if (me().indicator()) remove(me().indicator());
    add(me().indicator = newValue);
    me().bindIndicator();
    return *this;
}

ListItem &ListItem::indicator(const Image &newValue)
{
    return indicator(ImageView{newValue});
}

} // namespace cc
