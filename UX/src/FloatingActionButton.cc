/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/FloatingActionButton>
#include <cc/ImageView>
#include <cc/Icon>
#include <cc/Label>
#include <cc/Box>
#include <cc/Shadow>
#include <cc/DEBUG>

namespace cc {

struct FloatingActionButton::State final: public InputControl::State
{
    static int flavorHeight(Flavor flavor)
    {
        int height = 56;
        switch (flavor) {
        case Flavor::Tiny:
            height = 40;
            break;
        case Flavor::Extended:
            height = 48;
            break;
        case Flavor::Regular:
        default:;
        };
        return height;
    }

    State(Flavor flavor = Flavor::Regular, const String &text = String{}):
        flavor_{flavor},
        d_{sp(flavorHeight(flavor))},
        s_{d_, d_}
    {
        textColor([this]{ return theme().floatingActionButtonTextColor(); });

        add(
            Box{}
            .associate(&box_)
            .color([this]{ return theme().floatingActionButtonColor(); })
            .radius(d_ / 2)
            .size(s_)
            .pos([this]{ return pressed() ? Point{0, sp(3)} : Point{0, 0}; })
        );

        if (flavor == Flavor::Extended) {
            box_
            .add(
                Label{text}
                .associate(&label_)
                .color([this]{ return theme().floatingActionButtonTextColor(); })
                .centerLeft([this]{ return Point{sp(48), sp(24)}; })
            )
            .size(fixedSize());
        }

        box_
        .size(fixedSize())
        .decoration(
            Shadow{}
            .color([this]{ return theme().floatingActionButtonShadowColor(); })
            .offset(Step{0, sp(3)})
            .blurRadius(sp(4))
            .opacity([this]{ return !pressed(); })
        );

        size(box_.size());
    }

    Size fixedSize() const { return s_ + Size{label_ ? sp(20) + label_.width()  : 0., 0.}; }
    Size minSize() const override { return fixedSize(); }
    Size maxSize() const override { return fixedSize(); }

    void icon(Ideographic ch)
    {
        icon(
            Icon{ch, sp(24), textColor()}
            .color([this]{ return textColor(); })
        );
    }

    void icon(const View &view)
    {
        if (iconView_ == view) return;
        if (iconView_) box_.remove(iconView_);
        iconView_ = view;
        box_.add(
            iconView_
            .center([this]{ return s_ / 2; })
            .paper([this]{ return box_.color(); })
        );
    }

    Color color() const { return box_.color(); }
    void color(Color newValue) { box_.color(newValue); }
    void color(Definition<Color> &&f) { box_.color(move(f)); }

    const Flavor flavor_;
    const double d_;
    Size s_;
    Property<Color> textColor;
    View iconView_;
    Box box_;
    Label label_;
};

FloatingActionButton::FloatingActionButton():
    InputControl{onDemand<State>}
{}

FloatingActionButton::FloatingActionButton(Flavor flavor):
    InputControl{new State{flavor}}
{}

FloatingActionButton::FloatingActionButton(const String &text):
    InputControl{new State{Flavor::Extended, text}}
{}

FloatingActionButton &FloatingActionButton::associate(Out<FloatingActionButton> self)
{
    return View::associate<FloatingActionButton>(self);
}

FloatingActionButton &FloatingActionButton::icon(Ideographic ch)
{
    me().icon(ch);
    return *this;
}

FloatingActionButton &FloatingActionButton::icon(const Image &image)
{
    me().icon(ImageView{image});
    return *this;
}

FloatingActionButton &FloatingActionButton::icon(const View &view)
{
    me().icon(view);
    return *this;
}

Color FloatingActionButton::color() const
{
    return me().color();
}

FloatingActionButton &FloatingActionButton::color(Color newValue)
{
    me().color(newValue);
    return *this;
}

FloatingActionButton &FloatingActionButton::color(Definition<Color> &&f)
{
    me().color(move(f));
    return *this;
}

Color FloatingActionButton::textColor() const
{
    return me().textColor();
}

FloatingActionButton &FloatingActionButton::textColor(Color newValue)
{
    me().textColor(newValue);
    return *this;
}

FloatingActionButton &FloatingActionButton::textColor(Definition<Color> &&f)
{
    me().textColor(move(f));
    return *this;
}

FloatingActionButton::State &FloatingActionButton::me()
{
    return get<State>();
}

const FloatingActionButton::State &FloatingActionButton::me() const
{
    return get<State>();
}

} // namespace cc
