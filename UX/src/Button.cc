/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Button>
#include <cc/ImageView>
#include <cc/Icon>
#include <cc/Label>
#include <cc/Box>
#include <cc/Shadow>
#include <cc/DEBUG>

namespace cc {

struct Button::State final: public InputControl::State
{
    static int flavorHeight(ButtonStyle flavor)
    {
        int height = 56;
        switch (flavor) {
        case ButtonStyle::Regular:
        case ButtonStyle::TinyFAB:
            height = 40;
            break;
        case ButtonStyle::ExtendedFAB:
            height = 48;
            break;
        case ButtonStyle::FAB:
        default:;
        };
        return height;
    }

    State(ButtonStyle flavor = ButtonStyle::Regular, const String &text = String{}):
        flavor_{flavor},
        d_{sp(flavorHeight(flavor))},
        s_{d_, d_}
    {
        textColor([this]{ return theme().floatingActionButtonTextColor(flavor_); });

        add(
            Box{}
            .associate(&box_)
            .color([this]{ return theme().floatingActionButtonColor(flavor_); })
            .radius(d_ / 2)
            .size(s_)
            .pos([this]{ return pressed() ? Point{0, sp(3)} : Point{0, 0}; })
        );

        if (flavor == ButtonStyle::ExtendedFAB || flavor == ButtonStyle::Regular) {
            box_
            .add(
                Label{text}
                .associate(&label_)
                .color([this]{ return theme().floatingActionButtonTextColor(flavor_); })
                .centerLeft([this]{ return Point{sp(flavorHeight() / (iconView_().isNull() ? 2 : 1)), sp(flavorHeight() / 2)}; })
            )
            .size(fixedSize());
        }

        box_
        .size([this]{ return fixedSize(); })
        .decoration(
            Shadow{}
            .color([this]{ return theme().floatingActionButtonShadowColor(flavor_); })
            .offset(Step{0, sp(3)})
            .blurRadius(sp(4))
            .opacity([this]{ return !pressed(); })
        );

        size(box_.size());
    }

    int flavorHeight() const
    {
        return flavorHeight(flavor_);
    }

    Size fixedSize() const {
        return s_
            + Size{
                (label_ ? sp(flavorHeight() / 2) + label_.width() : 0.) +
                (iconView_().isNull() ? -sp(flavorHeight() / 2) : 0.),
                0.
            };
    }
    Size minSize() const override { return fixedSize(); }
    Size maxSize() const override { return fixedSize(); }

    void iconPicture(const Picture &picture)
    {
        iconView(
            Picture{picture}
            .color([this]{ return textColor(); })
        );
    }

    void iconView(const View &view)
    {
        if (iconView_() == view) return;
        if (!iconView_().isNull()) box_.remove(iconView_());
        iconView_(view);
        box_.add(
            iconView_()
            .center([this]{ return s_ / 2; })
            .paper([this]{ return box_.color(); })
        );
    }

    Color color() const { return box_.color(); }
    void color(Color newValue) { box_.color(newValue); }
    void color(Definition<Color> &&f) { box_.color(move(f)); }

    const ButtonStyle flavor_;
    const double d_;
    Size s_;
    Property<Color> textColor;
    Property<View> iconView_;
    Box box_;
    Label label_;
};

Button::Button():
    InputControl{onDemand<State>}
{}

Button::Button(ButtonStyle flavor, const String &text, const Picture &icon):
    InputControl{new State{flavor, text}}
{
    if (!icon.isNull()) this->icon(icon);
}

Button::Button(ButtonStyle flavor, const Picture &icon):
    Button{flavor, String{}, icon}
{}

Button::Button(const String &text, const Picture &icon):
    Button{ButtonStyle::Regular, text, icon}
{}

Button::Button(const Picture &icon):
    Button{ButtonStyle::Regular, String{}, icon}
{}

Button &Button::associate(Out<Button> self)
{
    return View::associate<Button>(self);
}

Button &Button::icon(const Picture &newValue)
{
    me().iconPicture(newValue);
    return *this;
}

Color Button::color() const
{
    return me().color();
}

Button &Button::color(Color newValue)
{
    me().color(newValue);
    return *this;
}

Button &Button::color(Definition<Color> &&f)
{
    me().color(move(f));
    return *this;
}

Color Button::textColor() const
{
    return me().textColor();
}

Button &Button::textColor(Color newValue)
{
    me().textColor(newValue);
    return *this;
}

Button &Button::textColor(Definition<Color> &&f)
{
    me().textColor(move(f));
    return *this;
}

Button::State &Button::me()
{
    return get<State>();
}

const Button::State &Button::me() const
{
    return get<State>();
}

} // namespace cc
