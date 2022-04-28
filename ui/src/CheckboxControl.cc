/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/CheckboxControl>
#include <cc/Picture>
#include <cc/Box>

namespace cc {

struct CheckboxControl::State final: public InputControl::State
{
    State()
    {
        Box highlight;

        add(
            highlight
            .color([this]{
                int percent = 0;
                if (focus() || pressed()) percent = 8;
                else if (hover()) percent = 5;
                return basePaper().mixedWith(theme().switchFocusColor(), percent);
            })
            .radius([this]{ return d_; })
            .size([this]{ return size(); })
            .visible([this]{ return focus() || hover() || pressed(); })
        );

        add(
            Picture{Ideographic::CheckboxMarked, d_, theme().checkboxColor(true)}
            .centerInParent()
            .visible([this]{ return checked(); })
        );

        add(
            Picture{Ideographic::CheckboxBlankOutline, d_, theme().checkboxColor(false)}
            .centerInParent()
            .visible([this]{ return !checked(); })
        );

        onClicked([this]{
            checked(!checked());
        });

        onKeyPressed([this](const KeyEvent &event){
            if (
                event.keyCode() == KeyCode::Key_Plus ||
                event.keyCode() == KeyCode::KeyPad_Plus ||
                event.keyCode() == KeyCode::Key_Right ||
                event.keyCode() == KeyCode::Key_Up ||
                event.keyCode() == KeyCode::Key_Minus ||
                event.keyCode() == KeyCode::KeyPad_Minus ||
                event.keyCode() == KeyCode::Key_Left ||
                event.keyCode() == KeyCode::Key_Down ||
                event.keyCode() == KeyCode::Key_Space
            ) {
                checked(!checked());
                return true;
            }
            return false;
        });

        size([this]{ return preferredSize(); });
    }

    Size preferredSize() const override { return 2 * Size{d_, d_}; }
    Size minSize() const override { return preferredSize(); }
    Size maxSize() const override { return preferredSize(); }

    const double d_ { sp(24) };

    Property<bool> checked { false };
};

CheckboxControl::CheckboxControl():
    InputControl{onDemand<State>}
{}

CheckboxControl::CheckboxControl(Out<CheckboxControl> self):
    InputControl{new State}
{
    self = weak<CheckboxControl>();
}

bool CheckboxControl::value() const
{
    return me().checked();
}

CheckboxControl &CheckboxControl::value(bool newValue)
{
    me().checked(newValue);
    return *this;
}

CheckboxControl &CheckboxControl::value(Definition<bool> &&f)
{
    me().checked(std::move(f));
    return *this;
}

CheckboxControl::State &CheckboxControl::me()
{
    return View::me().as<State>();
}

const CheckboxControl::State &CheckboxControl::me() const
{
    return View::me().as<State>();
}

} // namespace cc
