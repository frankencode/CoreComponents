/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/NumberCellState>

namespace cc {

NumberCell::State::State()
{
    color([this]{
        return theme().inputFieldFillColor(pressed());
    });

    add(
        Box{}
        .associate(&box_)
        .color([this]{ return color(); })
        .radius([this]{ return sp(6); })
        .add(
            NumberInput{}
            .associate(&input_)
            .textAlign(TextAlign::Center)
            .paper([this]{ return box_.color(); })
            .centerInParent()
        )
        .size([this]{
            Size s = input_.size();
            s[0] += sp(24);
            s[1] += sp(16);
            return s;
        })
    );

    size([this]{
        return preferredSize();
    });

    padding(sp(8));

    input_.onAccepted([this]{
        input_.focus(false);
    });
}

Size NumberCell::State::preferredSize() const
{
    return box_.size();
}

Size NumberCell::State::minSize() const
{
    return preferredSize();
}

Size NumberCell::State::maxSize() const
{
    return preferredSize();
}

Control NumberCell::State::delegate() const
{
    if (!textInput()) return Control{};
    return input_;
}

bool NumberCell::State::cascade() const
{
    return false;
}

NumberCell::NumberCell():
    Control{onDemand<State>}
{}

NumberCell &NumberCell::associate(Out<NumberCell> self)
{
    return View::associate<NumberCell>(self);
}

double NumberCell::min() const
{
    return me().input_.min();
}

NumberCell &NumberCell::min(double newValue)
{
    me().autoConnect_ = false;
    me().input_.min(newValue);
    return *this;
}

NumberCell &NumberCell::min(Definition<double> &&f)
{
    me().autoConnect_ = false;
    me().input_.min(move(f));
    return *this;
}

double NumberCell::max() const
{
    return me().input_.max();
}

NumberCell &NumberCell::max(double newValue)
{
    me().autoConnect_ = false;
    me().input_.max(newValue);
    return *this;
}

NumberCell &NumberCell::max(Definition<double> &&f)
{
    me().autoConnect_ = false;
    me().input_.max(move(f));
    return *this;
}

double NumberCell::precision() const
{
    return me().input_.precision();
}

NumberCell &NumberCell::precision(double newValue)
{
    me().autoConnect_ = false;
    me().input_.precision(newValue);
    return *this;
}

NumberCell &NumberCell::precision(Definition<double> &&f)
{
    me().autoConnect_ = false;
    me().input_.precision(move(f));
    return *this;
}

int NumberCell::digits() const
{
    return me().input_.digits();
}

NumberCell &NumberCell::digits(int newValue)
{
    me().input_.digits(newValue);
    return *this;
}

NumberCell &NumberCell::digits(Definition<int> &&f)
{
    me().input_.digits(move(f));
    return *this;
}

char NumberCell::decimalPoint() const
{
    return me().input_.decimalPoint();
}

NumberCell &NumberCell::decimalPoint(char newValue)
{
    me().input_.decimalPoint(newValue);
    return *this;
}

NumberCell &NumberCell::decimalPoint(Definition<char> &&f)
{
    me().input_.decimalPoint(move(f));
    return *this;
}

Font NumberCell::font() const
{
    return me().input_.font();
}

NumberCell &NumberCell::font(Font newValue)
{
    me().input_.font(newValue);
    return *this;
}

NumberCell &NumberCell::font(Definition<Font> &&f)
{
    me().input_.font(move(f));
    return *this;
}

double NumberCell::value() const
{
    return me().input_.value();
}

NumberCell &NumberCell::value(double newValue)
{
    me().input_.value(newValue);
    return *this;
}

NumberCell &NumberCell::onValueChanged(Function<void()> &&f)
{
    me().input_.onValueChanged(move(f));
    return *this;
}

bool NumberCell::textInput() const
{
    return me().textInput();
}

NumberCell &NumberCell::textInput(bool newValue)
{
    me().textInput(newValue);
    return *this;
}

NumberCell &NumberCell::textInput(Definition<bool> &&f)
{
    me().textInput(move(f));
    return *this;
}

Color NumberCell::textColor() const
{
    return me().input_.textColor();
}

NumberCell &NumberCell::textColor(Color newValue)
{
    me().input_.textColor(newValue);
    return *this;
}

NumberCell &NumberCell::textColor(Definition<Color> &&f)
{
    me().input_.textColor(move(f));
    return *this;
}

Color NumberCell::color() const
{
    return me().color();
}

NumberCell& NumberCell::color(Color newValue)
{
    me().color(newValue);
    return *this;
}

NumberCell& NumberCell::color(Definition<Color> &&f)
{
    me().color(move(f));
    return *this;
}

NumberCell &NumberCell::onAccepted(Function<void()> &&f)
{
    me().input_.onAccepted(move(f));
    return *this;
}

NumberCell &NumberCell::onRejected(Function<void()> &&f)
{
    me().input_.onAccepted(move(f));
    return *this;
}

NumberCell::State &NumberCell::me()
{
    return View::me().as<State>();
}

const NumberCell::State &NumberCell::me() const
{
    return View::me().as<State>();
}

} // namespace cc
