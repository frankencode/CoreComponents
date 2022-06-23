/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/TouchButton>
#include <cc/Label>
#include <cc/Box>

namespace cc {

struct TouchButton::State final: public InputControl::State
{
    State(Style style = Style::Default, const String &text = String{}, const Picture &icon = Picture{}):
        style_{style},
        label_{text},
        icon_{icon}
    {
        size([this]{ return preferredSize(); });
        extraMargin([this]{ return (size()[0] - minSize()[0]) / 2; });

        add(
            box_
            .color([this]{
                Color c = theme().touchButtonColor(style_, pressed());
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
            .color([this]{ return theme().touchButtonTextColor(style_, pressed()); })
            .font([this,style]{
                return (style != Style::Text) ? cc::style().defaultFont() : cc::style().defaultMediumFont();
            })
            .paper([this]{
                if (box_.color().isValid()) return box_.color();
                return basePaper();
            })
            .centerRight([this]{
                return size() - Size{(icon_.isNull() ? d_/2 : sp(20)) + extraMargin(), d_/2};
            })
        );

        if (!icon_.isNull()) {
            box_.add(
                icon_
                .color([this]{ return label_.color(); })
                .paper([this]{ return label_.paper(); })
                .centerLeft([this]{ return Point{sp(12) + extraMargin(), d_/2}; })
            );
        }
    }

    Size minSize() const override
    {
        if (icon_.isNull())
            return Size{d_ + label_.width(), d_};

        return Size{
            sp(12) + icon_.width() + sp(8) + label_.width() + sp(20),
            d_
        };
    }

    Size maxSize() const override
    {
        return Size{
            std::numeric_limits<double>::max(),
            d_
        };
    }

    Size preferredSize() const override
    {
        Size s = minSize();
        if (autoExpand()) {
            s[0] = expandableWidth(s[0]);
        }
        return s;
    }

    const double d_ { sp(40) };
    Style style_;
    Property<bool> autoExpand { true };
    Property<double> extraMargin;
    Box box_;
    Label label_;
    Picture icon_;
};

TouchButton::TouchButton():
    InputControl{onDemand<State>}
{}

TouchButton::TouchButton(Style style, const String &text, const Picture &icon):
    InputControl{new State{style, text, icon}}
{}

TouchButton::TouchButton(const String &text, const Picture &icon):
    TouchButton{Style::Default, text, icon}
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

TouchButton::State &TouchButton::me()
{
    return get<State>();
}

const TouchButton::State &TouchButton::me() const
{
    return get<State>();
}

} // namespace cc
