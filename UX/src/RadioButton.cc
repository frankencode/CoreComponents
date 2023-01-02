/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/RadioButton>
#include <cc/RadioButtonControl>
#include <cc/Text>

namespace cc {

struct RadioButton::State final: public Control::State
{
    State(const String &text = ""):
        text_{text}
    {
        add(
            radio_
            .centerLeft([this]{ return Point{sp(m), sp(h / 2)}; })
        );

        add(
            text_
            .centerLeft([this]{ return radio_.centerRight() + Step{sp(m), 0}; })
        );

        size([this]{ return preferredSize(); });
    }

    Control delegate() const override
    {
        return radio_;
    }

    Size preferredSize() const override
    {
        Size s = minSize();
        s[0] = expandableWidth(s[0]);
        return s;
    }

    Size minSize() const override
    {
        return Size{sp(m) + radio_.width() + sp(m) + text_.width(), sp(h)};
    }

    Size maxSize() const override
    {
        return Size{std::numeric_limits<double>::max(), sp(h)};
    }

    static constexpr double m = 8;
    static constexpr double h = 48;

    RadioButtonControl radio_;
    Text text_;
};

RadioButton::RadioButton():
    Control{onDemand<State>}
{}

RadioButton::RadioButton(const String &text):
    Control{new State{text}}
{}

RadioButton &RadioButton::join(Out<RadioButtonGroup> group)
{
    me().radio_.join(group);
    return *this;
}

String RadioButton::text() const
{
    return me().text_.text();
}

RadioButton &RadioButton::text(const String &newValue)
{
    me().text_.text(newValue);
    return *this;
}

RadioButton &RadioButton::text(Definition<String> &&f)
{
    me().text_.text(move(f));
    return *this;
}

bool RadioButton::value() const
{
    return me().radio_.value();
}

RadioButton &RadioButton::value(bool newValue)
{
    me().radio_.value(newValue);
    return *this;
}

RadioButton &RadioButton::onSelected(Function<void()> &&f)
{
    me().radio_.onSelected(move(f));
    return *this;
}

RadioButton::State &RadioButton::me()
{
    return get<State>();
}

const RadioButton::State &RadioButton::me() const
{
    return get<State>();
}

} // namespace cc
