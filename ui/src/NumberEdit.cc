/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/NumberEdit>
#include <cc/NumberInput>
#include <cc/NumberInputState>

namespace cc {

NumberEdit::NumberEdit(const String &label):
    LineEdit{NumberInput{}, label}
{}

NumberEdit &NumberEdit::associate(Out<NumberEdit> self)
{
    return View::associate(self);
}

double NumberEdit::min() const
{
    return input().as<NumberInput>().min();
}

NumberEdit &NumberEdit::min(double newValue)
{
    input().as<NumberInput>().min(newValue);
    return *this;
}

NumberEdit &NumberEdit::min(Definition<double> &&f)
{
    input().as<NumberInput>().min(move(f));
    return *this;
}

double NumberEdit::max() const
{
    return input().as<NumberInput>().max();
}

NumberEdit &NumberEdit::max(double newValue)
{
    input().as<NumberInput>().max(newValue);
    return *this;
}

NumberEdit &NumberEdit::max(Definition<double> &&f)
{
    input().as<NumberInput>().max(move(f));
    return *this;
}

double NumberEdit::precision() const
{
    return input().as<NumberInput>().precision();
}

NumberEdit &NumberEdit::precision(double newValue)
{
    input().as<NumberInput>().precision(newValue);
    return *this;
}

NumberEdit &NumberEdit::precision(Definition<double> &&f)
{
    input().as<NumberInput>().precision(move(f));
    return *this;
}

char NumberEdit::decimalPoint() const
{
    return input().as<NumberInput>().decimalPoint();
}

NumberEdit &NumberEdit::decimalPoint(char newValue)
{
    input().as<NumberInput>().decimalPoint(newValue);
    return *this;
}

NumberEdit &NumberEdit::decimalPoint(Definition<char> &&f)
{
    input().as<NumberInput>().decimalPoint(move(f));
    return *this;
}

double NumberEdit::value() const
{
    return input().as<NumberInput>().value();
}

NumberEdit &NumberEdit::value(double newValue)
{
    input().as<NumberInput>().value(newValue);
    return *this;
}

} // namespace cc
