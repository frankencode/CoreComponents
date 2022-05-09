/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ElevatedButton>
#include <cc/Label>
#include <cc/Icon>
#include <cc/Box>
#include <cc/Shadow>
#include <cc/DEBUG>

namespace cc {

struct ElevatedButton::State final: public InputControl::State
{
    State();

    void setLeading(View view);

    Size preferredSize() const override;
    Size minSize() const override;
    Size maxSize() const override;

    Property<View> leading;
    Box box_;
    View leading_;
    Label label_;
};

ElevatedButton::State::State()
{
    add(
        Box{&box_}
        .radius([this]{ return sp(20); })
        .color([this]{
            return focus() ?
                theme().elevatedButtonFocusColor(pressed()) :
                theme().elevatedButtonColor(pressed());
        })
        .add(
            Label{&label_}
            .color([this]{
                return focus() ?
                    theme().elevatedButtonTextColor(pressed()) :
                    theme().elevatedButtonFocusTextColor(pressed());
            })
            .paper([this]{ return box_.color(); })
            .pos([this]{ return Point{sp(20) + (leading() ? leading().width() : 0) + sp(10), sp(20) - label_.height() / 2}; })
        )
        .size([this]{ return size(); })
        .decoration(
            Shadow{}
            .color([this]{ return theme().elevatedButtonShadowColor(); })
            .offset(Step{0, sp(2)})
            .blurRadius(sp(3))
        )
    );

    size([this]{ return preferredSize(); });
}

void ElevatedButton::State::setLeading(View view)
{
    if (leading() == view) return;
    if (leading()) remove(leading());
    view
        .centerLeft([this]{ return Point{sp(20)}; })
        .paper([this]{ return box_.color(); });
    add(view);
    leading(view);
}

Size ElevatedButton::State::preferredSize() const
{
    return Size{label_.size()[0] + (leading() ? leading().size()[0] + sp(10) : 0) + sp(40) + sp(leading() ? 5 : 10) , sp(40)};
}

Size ElevatedButton::State::minSize() const
{
    return preferredSize();
}

Size ElevatedButton::State::maxSize() const
{
    return Size{ std::numeric_limits<double>::max(), sp(40) };
}

ElevatedButton::ElevatedButton():
    InputControl{onDemand<State>}
{}

ElevatedButton::ElevatedButton(Out<ElevatedButton> self):
    InputControl{new State}
{}

String ElevatedButton::text() const
{
    return me().label_.text();
}

ElevatedButton &ElevatedButton::text(const String &newValue)
{
    me().label_.text(newValue);
    return *this;
}

ElevatedButton &ElevatedButton::text(Definition<String> &&f)
{
    me().label_.text(move(f));
    return *this;
}

ElevatedButton &ElevatedButton::icon(Ideographic ch)
{
    me().setLeading(Icon{ch, sp(20), me().label_.color()});
    return *this;
}

ElevatedButton &ElevatedButton::leading(const View &view)
{
    me().setLeading(view);
    return *this;
}

ElevatedButton::State &ElevatedButton::me()
{
    return View::me().as<State>();
}

const ElevatedButton::State &ElevatedButton::me() const
{
    return View::me().as<State>();
}

} // namespace cc
