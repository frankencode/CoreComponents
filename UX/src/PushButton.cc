/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/PushButtonState>
#include <cc/ImageView>
#include <cc/Shadow>

namespace cc {

int PushButton::State::styleHeight(Style style)
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

PushButton::State::State(Style style, const String &text, const Picture &icon):
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

    if (!icon.isNull()) iconPicture(icon);

    size([this]{ return getSize(); });
    padding(sp(8));
}

int PushButton::State::styleHeight() const
{
    return styleHeight(style_);
}

Size PushButton::State::getSize() const
{
    return s_
        + Size{
            (label_ ? sp(styleHeight() / 2) + label_.width() : 0.) +
            ((iconView_().isNull() && style_ != Style::TinyFloating) ? -sp(styleHeight() / 2) : 0.),
            0.
        };
}

Size PushButton::State::minSize() const
{
    return getSize();
}

Size PushButton::State::maxSize() const
{
    return getSize();
}

void PushButton::State::iconPicture(const Picture &picture)
{
    iconView(
        Picture{picture}
        .color([this]{ return textColor(); })
    );
}

void PushButton::State::iconView(const View &view)
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

PushButton::PushButton():
    InputControl{onDemand<State>}
{}

PushButton::PushButton(Style style, const String &text, const Picture &icon):
    InputControl{new State{style, text, icon}}
{}

PushButton::PushButton(Style style, const Picture &icon):
    PushButton{style, String{}, icon}
{}

PushButton::PushButton(const String &text, const Picture &icon):
    PushButton{Style::Default, text, icon}
{}

PushButton::PushButton(const Picture &icon):
    PushButton{Style::Default, String{}, icon}
{}

PushButton::PushButton(CreateState onDemand):
    InputControl{onDemand}
{}

PushButton::PushButton(State *newState):
    InputControl{newState}
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

PushButton &PushButton::onTriggered(Fun<void()> &&f)
{
    me().onClicked(move(f));
    return *this;
}

PushButton &PushButton::operator()(Fun<void()> &&f)
{
    return onTriggered(move(f));
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
