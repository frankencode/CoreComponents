/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/TouchButtonState>
#include <cc/Label>
#include <cc/Box>

namespace cc {

TouchButton::State::State(Style style, const String &text, const Picture &icon):
    style_{style},
    label_{text},
    icon_{icon}
{
    size([this]{ return preferredSize(); });
    padding(sp(8));

    extraMargin([this]{ return (size()[0] - minSize()[0]) / 2; });

    add(
        box_
        .color([this]{
            Color c = theme().touchButtonColor(style_, pressed());
            if (!c.isValid()) c = paper();
            if (!c.isValid()) c = basePaper();
            return c;
        })
        .border([this]{
            return theme().touchButtonBorder(style_, pressed());
        })
        .radius((style != Style::Text) ? d_ / 2 : 0.)
        .size([this]{ return size(); })
    );

    add(
        label_
        .font([this,style]{
            return (style != Style::Text) ? cc::style().defaultFont() : cc::style().defaultMediumFont();
        })
        .color([this]{ return theme().touchButtonTextColor(style_, pressed()); })
        .paper([this]{
            if (box_.color().isValid()) return box_.color();
            return basePaper();
        })
        .centerRight([this]{
            return size() - Size{(icon_.isNull() ? d_/2 : sp(20)) + extraMargin(), d_/2};
        })
        .visible([this]{
            return label_.text() != "" && visible();
        })
    );

    if (!icon_.isNull()) {
        box_.add(
            icon_
            .color([this]{ return theme().touchButtonTextColor(style_, pressed()); })
            .paper([this]{
                if (box_.color().isValid()) return box_.color();
                return basePaper();
            })
            .centerLeft([this]{ return Point{sp(12) + extraMargin(), d_/2}; })
        );
    }
}

Size TouchButton::State::minSize() const
{
    if (icon_.isNull())
        return Size{d_ + label_.width(), d_};

    return Size{
        sp(12) + icon_.width() + (label_.visible() ? sp(8) + label_.width() + sp(20) : sp(12)),
        d_
    };
}

Size TouchButton::State::maxSize() const
{
    return Size{
        std::numeric_limits<double>::max(),
        d_
    };
}

Size TouchButton::State::preferredSize() const
{
    Size s = minSize();
    if (autoExpand()) {
        s[0] = expandableWidth(s[0]);
    }
    return s;
}

TouchButton::TouchButton():
    InputControl{onDemand<State>}
{}

TouchButton::TouchButton(Style style, const String &text, const Picture &icon):
    InputControl{new State{style, text, icon}}
{}

TouchButton::TouchButton(const String &text, const Picture &icon):
    TouchButton{Style::Default, text, icon}
{}

TouchButton::TouchButton(State *newState):
    InputControl{newState}
{}

TouchButton::TouchButton(CreateState onDemand):
    InputControl{onDemand}
{}

TouchButton &TouchButton::associate(Out<TouchButton> self)
{
    return View::associate<TouchButton>(self);
}

String TouchButton::text() const
{
    return me().label_.text();
}

TouchButton &TouchButton::text(const String &newValue)
{
    me().label_.text(newValue);
    return *this;
}

TouchButton &TouchButton::text(Definition<String> &&f)
{
    me().label_.text(move(f));
    return *this;
}

TouchButton &TouchButton::autoExpand(bool on)
{
    me().autoExpand(on);
    return *this;
}

TouchButton &TouchButton::onTriggered(Function<void()> &&f)
{
    me().onClicked(move(f));
    return *this;
}

TouchButton &TouchButton::operator()(Function<void()> &&f)
{
    me().onClicked(move(f));
    return *this;
}

TouchButton::State &TouchButton::me()
{
    return get<State>();
}

const TouchButton::State &TouchButton::me() const
{
    return get<State>();
}

} // namespace cc
