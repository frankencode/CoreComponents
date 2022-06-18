/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/RadioButtonControl>
#include <cc/Picture>
#include <cc/Box>

namespace cc {

struct RadioButtonControl::State final: public InputControl::State
{
    State()
    {
        size(preferredSize());

        Box highlight;

        add(
            highlight
            .color([this]{
                int percent = 0;
                if (focus() || pressed()) percent = 8;
                else if (hover()) percent = 5;
                return basePaper().mixedWith(theme().radioboxFocusColor(), percent);
            })
            .radius([this]{ return d_; })
            .size([this]{ return size(); })
            .visible([this]{ return focus() || hover() || pressed(); })
        );

        add(
            Picture{Ideographic::RadioboxMarked, d_, theme().radioboxColor(true)}
            .paper(Color::Transparent)
            .centerInParent()
            .visible([this]{ return value(); })
        );

        add(
            Picture{Ideographic::RadioboxBlank, d_, theme().radioboxColor(false)}
            .paper(Color::Transparent)
            .centerInParent()
            .visible([this]{ return !value(); })
        );

        onClicked([this]{
            value(!value());
        });

        onKeyPressed([this](const KeyEvent &event){
            if (
                event.keyCode() == KeyCode::Key_Plus ||
                event.keyCode() == KeyCode::KeyPad_Plus ||
                event.keyCode() == KeyCode::Key_Minus ||
                event.keyCode() == KeyCode::KeyPad_Minus ||
                event.keyCode() == KeyCode::Key_Space
            ) {
                value(!value());
                return true;
            }
            return false;
        });

        attach([this]{
            if (value()) onSelected();
        });
    }

    Size preferredSize() const override { return 2 * Size{d_, d_}; }
    Size minSize() const override { return preferredSize(); }
    Size maxSize() const override { return preferredSize(); }

    const double d_ { sp(24) };

    Property<bool> value;
    Trigger onSelected;
};

RadioButtonControl::RadioButtonControl():
    InputControl{onDemand<State>}
{}

RadioButtonControl &RadioButtonControl::associate(Out<RadioButtonControl> self)
{
    return View::associate<RadioButtonControl>(self);
}

RadioButtonControl &RadioButtonControl::join(Out<RadioButtonGroup> group)
{
    group->join(*this);
    return *this;
}

bool RadioButtonControl::value() const
{
    return me().value();
}

RadioButtonControl &RadioButtonControl::value(bool newValue)
{
    me().value(newValue);
    return *this;
}

RadioButtonControl &RadioButtonControl::onSelected(Fun<void()> &&f)
{
    me().onSelected(move(f));
    return *this;
}

RadioButtonControl::State &RadioButtonControl::me()
{
    return get<State>();
}

const RadioButtonControl::State &RadioButtonControl::me() const
{
    return get<State>();
}

} // namespace cc
