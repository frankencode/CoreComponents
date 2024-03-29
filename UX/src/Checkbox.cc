/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/Checkbox>
#include <cc/CheckboxControl>
#include <cc/Text>

namespace cc {

struct Checkbox::State final: public Control::State
{
    State(const String &text = ""):
        text_{text}
    {
        add(
            checkbox_
            .centerLeft([this]{ return Point{sp(checkbox_.isGrouped() ? 4 * m : m), sp(h / 2)}; })
        );

        add(
            text_
            .centerLeft([this]{ return checkbox_.centerRight() + Step{sp(m), 0}; })
        );

        size([this]{ return preferredSize(); });
    }

    Control delegate() const override
    {
        return checkbox_;
    }

    Size preferredSize() const override
    {
        Size s = minSize();
        s[0] = expandableWidth(s[0]);
        return s;
    }

    Size minSize() const override
    {
        return Size{sp(m) + checkbox_.width() + sp(m) + text_.width(), sp(h)};
    }

    Size maxSize() const override
    {
        return Size{std::numeric_limits<double>::max(), sp(h)};
    }

    static constexpr double m = 8;
    static constexpr double h = 48;

    CheckboxControl checkbox_;
    Text text_;
};

Checkbox::Checkbox():
    Control{onDemand<State>}
{}

Checkbox::Checkbox(const String &text):
    Control{new State{text}}
{}

Checkbox &Checkbox::associate(Out<Checkbox> self)
{
    return View::associate<Checkbox>(self);
}

String Checkbox::text() const
{
    return me().text_.text();
}

Checkbox &Checkbox::text(const String &newValue)
{
    me().text_.text(newValue);
    return *this;
}

Checkbox &Checkbox::text(Definition<String> &&f)
{
    me().text_.text(move(f));
    return *this;
}

bool Checkbox::value() const
{
    return me().checkbox_.value();
}

Checkbox &Checkbox::value(bool newValue)
{
    me().checkbox_.value(newValue);
    return *this;
}

Checkbox &Checkbox::onValueChanged(Function<void()> &&f)
{
    me().checkbox_.onValueChanged(move(f));
    return *this;
}

Checkbox &Checkbox::groupUnder(const Checkbox &groupLeader)
{
    me().checkbox_.groupUnder(Checkbox{groupLeader}.me().checkbox_);
    return *this;
}

Checkbox::State &Checkbox::me()
{
    return View::me().as<State>();
}

const Checkbox::State &Checkbox::me() const
{
    return View::me().as<State>();
}

} // namespace cc
