/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/NumberField>
#include <cc/ui/NumberInput>
#include <cc/ui/NumberInputState>

namespace cc::ui {

NumberField::NumberField(const String &label, Out<NumberField> self):
    TextField{NumberInput{}, label}
{
    self = weak<NumberField>();
}

double NumberField::min() const
{
    return input().as<NumberInput>().min();
}

NumberField &NumberField::min(double newValue)
{
    input().as<NumberInput>().min(newValue);
    return *this;
}

NumberField &NumberField::min(Definition<double> &&f)
{
    input().as<NumberInput>().min(std::move(f));
    return *this;
}

double NumberField::max() const
{
    return input().as<NumberInput>().max();
}

NumberField &NumberField::max(double newValue)
{
    input().as<NumberInput>().max(newValue);
    return *this;
}

NumberField &NumberField::max(Definition<double> &&f)
{
    input().as<NumberInput>().max(std::move(f));
    return *this;
}

double NumberField::precision() const
{
    return input().as<NumberInput>().precision();
}

NumberField &NumberField::precision(double newValue)
{
    input().as<NumberInput>().precision(newValue);
    return *this;
}

NumberField &NumberField::precision(Definition<double> &&f)
{
    input().as<NumberInput>().precision(std::move(f));
    return *this;
}

char NumberField::decimalPoint() const
{
    return input().as<NumberInput>().decimalPoint();
}

NumberField &NumberField::decimalPoint(char newValue)
{
    input().as<NumberInput>().decimalPoint(newValue);
    return *this;
}

NumberField &NumberField::decimalPoint(Definition<char> &&f)
{
    input().as<NumberInput>().decimalPoint(std::move(f));
    return *this;
}

double NumberField::value() const
{
    return input().as<NumberInput>().value();
}

NumberField &NumberField::value(double newValue)
{
    input().as<NumberInput>().value(newValue);
    return *this;
}

} // namespace cc::ui
