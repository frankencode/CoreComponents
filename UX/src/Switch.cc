/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Switch>
#include <cc/SwitchControl>
#include <cc/Text>

namespace cc {

struct Switch::State final: public Control::State
{
    State(const String &text = ""):
        text_{text}
    {
        add(
            text_
            .centerLeft([this]{ return Point{sp(m), sp(h/2)}; })
        );

        add(
            switch_
            .centerRight([this]{ return Point{width(), sp(h/2)}; })
        );

        size([this]{ return preferredSize(); });
    }

    Control delegate() const override
    {
        return switch_;
    }

    Size preferredSize() const override
    {
        Size s = minSize();
        s[0] = expandableWidth(s[0]);
        return s;
    }

    Size minSize() const override
    {
        return Size{sp(m) + text_.width() + sp(m) + switch_.width(), sp(h)};
    }

    Size maxSize() const override
    {
        return Size{std::numeric_limits<double>::max(), sp(h)};
    }

    static constexpr double m = 8;
    static constexpr double h = 48;
    Text text_;
    SwitchControl switch_;
};

Switch::Switch():
    Control{onDemand<State>}
{}

Switch::Switch(const String &text):
    Control{new State{text}}
{}

Switch &Switch::associate(Out<Switch> self)
{
    return View::associate<Switch>(self);
}

String Switch::text() const
{
    return me().text_.text();
}

Switch &Switch::text(const String &newValue)
{
    me().text_.text(newValue);
    return *this;
}

Switch &Switch::text(Definition<String> &&f)
{
    me().text_.text(move(f));
    return *this;
}

bool Switch::value() const
{
    return me().switch_.value();
}

Switch &Switch::value(bool newValue)
{
    me().switch_.value(newValue);
    return *this;
}

Switch &Switch::value(Definition<bool> &&f)
{
    me().switch_.value(move(f));
    return *this;
}

Switch &Switch::onValueChanged(Fun<void()> &&f)
{
    me().switch_.onValueChanged(move(f));
    return *this;
}

Switch &Switch::onUserInput(Fun<void()> &&f)
{
    me().switch_.onUserInput(move(f));
    return *this;
}

Switch::State &Switch::me()
{
    return View::me().as<State>();
}

const Switch::State &Switch::me() const
{
    return View::me().as<State>();
}

} // namespace cc
