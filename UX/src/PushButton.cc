/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/PushButton>
#include <cc/ImageView>
#include <cc/Label>
#include <cc/Box>
#include <cc/Shadow>
#include <cc/DEBUG>

namespace cc {

struct PushButton::State final: public InputControl::State
{
    static int styleHeight(Style style)
    {
        int height = 56;
        switch (style) {
        case Style::Elevated:
        case Style::TinyFloating:
            height = 40;
            break;
        case Style::FloatingWithLabel:
            height = 48;
            break;
        case Style::Floating:
        default:;
        };
        return height;
    }

    State(Style style = Style::Default, const String &text = String{}):
        style_{style},
        d_{sp(styleHeight(style))},
        s_{d_, d_}
    {
        textColor([this]{ return theme().pushButtonTextColor(style_); });

        add(
            Box{}
            .associate(&box_)
            .color([this]{ return theme().pushButtonColor(style_); })
            .radius(d_ / 2)
            .size(s_)
            .pos([this]{ return pressed() ? Point{0, sp(3)} : Point{0, 0}; })
        );

        if (style == Style::FloatingWithLabel || style == Style::Elevated) {
            box_
            .add(
                Label{text}
                .associate(&label_)
                .color([this]{ return theme().pushButtonTextColor(style_); })
                .centerLeft([this]{ return Point{sp(styleHeight() / (iconView_().isNull() ? 2 : 1)), sp(styleHeight() / 2)}; })
            )
            .size(getSize());
        }

        box_
        .size([this]{ return getSize(); })
        .decoration(
            Shadow{}
            .color([this]{ return theme().pushButtonShadowColor(style_); })
            .offset(Step{0, sp(3)})
            .blurRadius(sp(4))
            .opacity([this]{ return !pressed(); })
        );

        size([this]{ return getSize();});
    }

    int styleHeight() const
    {
        return styleHeight(style_);
    }

    Size getSize() const {
        return s_
            + Size{
                (label_ ? sp(styleHeight() / 2) + label_.width() : 0.) +
                ((iconView_().isNull() && style_ != Style::TinyFloating) ? -sp(styleHeight() / 2) : 0.),
                0.
            };
    }
    Size minSize() const override { return getSize(); }
    Size maxSize() const override { return getSize(); }

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

    const Style style_;
    const double d_;
    Size s_;
    Property<Color> textColor;
    Property<View> iconView_;
    Box box_;
    Label label_;
};

PushButton::PushButton():
    InputControl{onDemand<State>}
{}

PushButton::PushButton(Style style, const String &text, const Picture &icon):
    InputControl{new State{style, text}}
{
    if (!icon.isNull()) me().iconPicture(icon);
}

PushButton::PushButton(Style style, const Picture &icon):
    PushButton{style, String{}, icon}
{}

PushButton::PushButton(const String &text, const Picture &icon):
    PushButton{Style::Default, text, icon}
{}

PushButton::PushButton(const Picture &icon):
    PushButton{Style::Default, String{}, icon}
{}

PushButton &PushButton::associate(Out<PushButton> self)
{
    return View::associate<PushButton>(self);
}

PushButton &PushButton::text(const String &newValue)
{
    me().label_.text(newValue);
    return *this;
}

String PushButton::text() const
{
    return me().label_.text();
}

PushButton &PushButton::text(Definition<String> &&f)
{
    me().label_.text(move(f));
    return *this;
}

PushButton &PushButton::operator()(Fun<void()> &&f)
{
    me().onClicked(move(f));
    return *this;
}

PushButton::State &PushButton::me()
{
    return get<State>();
}

const PushButton::State &PushButton::me() const
{
    return get<State>();
}

} // namespace cc
