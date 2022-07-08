/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
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

void ListItem::State::icon(const View &newValue)
{
    if (icon_()) remove(icon_());
    icon_(newValue);
    add(icon_());
    bindIcon();
}

void ListItem::State::title(const View &newValue)
{
    if (title_()) remove(title_());
    title_(newValue);
    add(title_());
    bindTitle();
}

void ListItem::State::indicator(const View &newValue)
{
    if (indicator_()) remove(indicator_());
    indicator_(newValue);
    add(indicator_());
    bindIndicator();
}

Size ListItem::State::minSize() const
{
    return Size{
        sp(16) +
        (icon_() ? icon_().width() + sp(16) : 0) +
        (title_() ? title_().minSize()[0] + sp(16) : 0) +
        (indicator_() ? indicator_().width() + sp(16) : 0),
        style().itemHeight2()
    };
}

void ListItem::State::bindIcon()
{
    icon_().pos([this]{
        return Point{
            sp(16),
            (height() - icon_().height()) / 2
        };
    });
}

void ListItem::State::bindTitle()
{
    title_().pos([this]{
        return Point{
            (icon_() ? icon_().right() : 0) + sp(16),
            std::round((height() - title_().height()) / 2)
        };
    });

    title_().size([this]{
        Size minItemSize = minSize();
        if (minItemSize[0] <= size()[0]) return title_().minSize();
        return title_().minSize() - Size{minItemSize[0] - size()[0], 0};
    });
}

void ListItem::State::bindIndicator()
{
    indicator_().pos([this]{
        return Point{
            width() - indicator_().width() - sp(16),
            std::round((height() - indicator_().height()) / 2)
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
    me().icon(newValue);
    return *this;
}

ListItem &ListItem::icon(const Image &newValue)
{
    return icon(ImageView{newValue});
}

ListItem &ListItem::title(const View &newValue)
{
    me().title(newValue);
    return *this;
}

ListItem &ListItem::title(const String &newValue)
{
    return title(Label{newValue});
}

ListItem &ListItem::indicator(const View &newValue)
{
    me().indicator(newValue);
    return *this;
}

ListItem &ListItem::indicator(const Image &newValue)
{
    return indicator(ImageView{newValue});
}

} // namespace cc
