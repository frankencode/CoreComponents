/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/NumberCellState>
#include <cc/DEBUG>

namespace cc {

NumberCell::State::State()
{
    add(
        Box{}
        .associate(&box_)
        .color([this]{ return theme().inputFieldFillColor(pressed()); })
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
            s[1] += sp(12);
            return s;
        })
    );

    size([this]{
        return preferredSize();
    });

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
    return input_;
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

NumberCell &NumberCell::onValueChanged(Fun<void()> &&f)
{
    me().input_.onValueChanged(move(f));
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

const NumberCell::State *NumberCell::operator->() const
{
    return &me();
}

} // namespace cc
