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
    State()
    {
        add(
            Box{}
            .associate(&box_)
            .radius([this]{ return sp(20); })
            .color([this]{
                return focus() ?
                    theme().elevatedButtonFocusColor() :
                    theme().elevatedButtonColor();
            })
            .add(
                Label{}
                .associate(&label_)
                .color([this]{
                    return focus() ?
                        theme().elevatedButtonTextColor() :
                        theme().elevatedButtonFocusTextColor();
                })
                .paper([this]{ return box_.color(); })
                .pos([this]{ return Point{sp(20) + (leading() ? leading().width() : 0) + sp(10), sp(20) - label_.height() / 2}; })
            )
            .size([this]{ return size(); })
            .pos([this]{ return pressed() ? Point{0, sp(3)} : Point{0, 0}; })
            .decoration(
                Shadow{}
                .color([this]{ return theme().elevatedButtonShadowColor(); })
                .offset(Step{0, sp(3)})
                .blurRadius(sp(4))
                .opacity([this]{ return !pressed(); })
            )
        );

        size([this]{ return preferredSize(); });
    }

    void setLeading(View view)
    {
        if (leading() == view) return;
        if (leading()) remove(leading());
        view
            .centerLeft([this]{ return Point{sp(20)}; })
            .paper([this]{ return box_.color(); });
        box_.add(view);
        leading(view);
    }

    Size preferredSize() const
    {
        return Size{label_.size()[0] + (leading() ? leading().size()[0] + sp(10) : 0) + sp(40) + sp(leading() ? 5 : 10) , sp(40)};
    }

    Size minSize() const
    {
        return preferredSize();
    }

    Size maxSize() const
    {
        return Size{ std::numeric_limits<double>::max(), sp(40) };
    }

    Property<View> leading;
    Box box_;
    Label label_;
};

ElevatedButton::ElevatedButton():
    InputControl{onDemand<State>}
{}

ElevatedButton &ElevatedButton::associate(Out<ElevatedButton> self)
{
    return View::associate<ElevatedButton>(self);
}

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

ElevatedButton &ElevatedButton::icon(const View &view)
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
