/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/SwitchField>
#include <cc/ui/Text>
#include <cc/ui/Switch>

namespace cc::ui {

struct SwitchField::State final: public View::State
{
    State();

    Size preferredSize() const override;
    Size minSize() const override;
    Size maxSize() const override;

    static constexpr double m = 8;
    static constexpr double h = 48;
    Text text_;
    Switch switch_;
};

SwitchField::State::State()
{
    add(
        Text{&text_}
        .centerLeft([this]{ return Point{sp(m), sp(h/2)}; })
    );

    add(
        Switch{&switch_}
        .centerRight([this]{ return Point{width(), sp(h/2)}; })
    );

    size([this]{ return preferredSize(); });
}

Size SwitchField::State::preferredSize() const
{
    Size s = minSize();
    s[0] = expandableWidth(s[0]);
    return s;
}

Size SwitchField::State::minSize() const
{
    return Size{sp(m) + text_.width() + sp(m) + switch_.width(), sp(h)};
}

Size SwitchField::State::maxSize() const
{
    return Size{std::numeric_limits<double>::max(), sp(h)};
}

SwitchField::SwitchField():
    View{onDemand<State>}
{}

SwitchField::SwitchField(Out<SwitchField> self):
    View{new State}
{
    self = *this;
}

String SwitchField::text() const
{
    return me().text_.text();
}

SwitchField &SwitchField::text(const String &newValue)
{
    me().text_.text(newValue);
    return *this;
}

SwitchField &SwitchField::text(Definition<String> &&f)
{
    me().text_.text(std::move(f));
    return *this;
}

bool SwitchField::value() const
{
    return me().switch_.value();
}

SwitchField &SwitchField::value(bool newValue)
{
    me().switch_.value(newValue);
    return *this;
}

SwitchField &SwitchField::value(Definition<bool> &&f)
{
    me().switch_.value(std::move(f));
    return *this;
}

SwitchField::State &SwitchField::me()
{
    return View::me().as<State>();
}

const SwitchField::State &SwitchField::me() const
{
    return View::me().as<State>();
}

} // namespace cc::ui
