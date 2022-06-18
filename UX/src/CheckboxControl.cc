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
        size(preferredSize());

        Box highlight;

        add(
            highlight
            .color([this]{
                int percent = 0;
                if (focus() || pressed()) percent = 8;
                else if (hover()) percent = 5;
                return basePaper().mixedWith(theme().checkboxFocusColor(), percent);
            })
            .radius([this]{ return d_; })
            .size([this]{ return size(); })
            .visible([this]{ return focus() || hover() || pressed(); })
        );

        add(
            Picture{Ideographic::CheckboxMarked, d_, theme().checkboxColor(true)}
            .centerInParent()
            .visible([this]{ return state() == Marked; })
        );

        add(
            Picture{Ideographic::CheckboxIntermediate, d_, theme().checkboxColor(true)}
            .centerInParent()
            .visible([this]{ return state() == Intermediate; })
        );

        add(
            Picture{Ideographic::CheckboxBlankOutline, d_, theme().checkboxColor(false)}
            .centerInParent()
            .visible([this]{ return state() == Blank; })
        );

        onClicked([this]{
            setValue(!getValue());
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
                setValue(!getValue());
                return true;
            }
            return false;
        });
    }

    bool getValue() const
    {
        return state() != Blank;
    }

    void setValue(bool newValue)
    {
        if (state() == State::Blank) {
            state = Marked;
        }
        else {
            state = Blank;
        }

        for (CheckboxControl &member: groupMembers()) {
            member->state = state();
        }

        if (!groupLeader().isNull()) {
            int n = 0;
            for (const CheckboxControl &member: groupLeader()->groupMembers()) {
                n += (member->state != Blank);
            }
            if (n == 0) groupLeader()->state = Blank;
            else if (n == groupLeader()->groupMembers().count()) groupLeader()->state = Marked;
            else groupLeader()->state = Intermediate;
        }
    }

    Size preferredSize() const override { return 2 * Size{d_, d_}; }
    Size minSize() const override { return preferredSize(); }
    Size maxSize() const override { return preferredSize(); }

    const double d_ { sp(24) };

    enum Value {
        Blank,
        Marked,
        Intermediate
    };

    Property<Value> state { Blank };

    Property<CheckboxControl> groupLeader;
    Property<List<CheckboxControl>> groupMembers;
};

CheckboxControl::CheckboxControl():
    InputControl{onDemand<State>}
{}

CheckboxControl &CheckboxControl::associate(Out<CheckboxControl> self)
{
    return View::associate<CheckboxControl>(self);
}

bool CheckboxControl::value() const
{
    return me().getValue();
}

CheckboxControl &CheckboxControl::value(bool newValue)
{
    me().setValue(newValue);
    return *this;
}

CheckboxControl &CheckboxControl::groupUnder(CheckboxControl &groupLeader)
{
    me().groupLeader = groupLeader;
    auto members = groupLeader->groupMembers();
    members << *this;
    groupLeader->groupMembers = members;
    return *this;
}

bool CheckboxControl::isGrouped() const
{
    return !me().groupLeader().isNull();
}

CheckboxControl::State &CheckboxControl::me()
{
    return get<State>();
}

const CheckboxControl::State &CheckboxControl::me() const
{
    return get<State>();
}

CheckboxControl::State *CheckboxControl::operator->()
{
    return &me();
}

const CheckboxControl::State *CheckboxControl::operator->() const
{
    return &me();
}

} // namespace cc
