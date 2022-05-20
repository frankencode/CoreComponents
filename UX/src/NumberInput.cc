/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/NumberInput>
#include <cc/NumberInputState>
#include <cc/FontMetrics>
#include <cc/str>
#include <cmath>

namespace cc {

NumberInput::State::State()
{
    nf([this]{
        int n = static_cast<int>(std::ceil(std::log10(precision())));
        if (n > 0) n = 0;
        else if (n < 0) n = -n;
        return n;
    });

    sizerText([this]{
        String minText = str(min());
        String maxText = str(max());
        return minText.count() < maxText.count() ? maxText : minText;
    });

    value.onChanged([this]{
        updateText();
    });

    filter_ = [this](Range range, InOut<String> chunk){
        if (
            text().count() + chunk->count() - selection().count() >
            sizerText().count() - (min() < 0 && !text().startsWith('-'))
        ) {
            return true;
        }
        bool allowMinus = (min() < 0) && !text().startsWith('-');
        for (char &ch: chunk()) {
            if (ch == '.' || ch == ',') {
                ch = decimalPoint();
            }
            if (!(
                ('0' <= ch && ch <= '9') ||
                (ch == '-' && allowMinus) ||
                (nf() > 0 && (ch == decimalPoint()))
            )) {
                return true;
            }
            allowMinus = false;
        }
        return false;
    };

    accept_ = [this]{
        String t = text();
        long j0 = 0;
        while (j0 < t.count() - 1 && t[j0] == '0') ++j0;
        t = t.copy(j0, t.count());
        if (decimalPoint() != '.') t.replace(decimalPoint(), '.');
        bool ok = false;
        double x = t.toDouble(&ok);
        if (!ok) {
            bool h = (text().count() == 0);
            updateText();
            if (h) selectAll();
            return false;
        }
        const double p = precision();
        double c = std::fmod(x, p);
        if (c > p) x -= c;
        if (x < min()) x = min();
        else if (max() < x) x = max();
        value(x);
        return true;
    };

    onRejected([this]{
        updateText();
    });

    focus.onChanged([this]{
        if (focus()) {
            updateText();
            selectAll();
        }
    });

    text(str(value()));
}

void NumberInput::State::updateText()
{
    if (min() <= value() && value() <= max())
        text(str(value()));
    else
        text("");
}

String NumberInput::State::str(double x) const
{
    String t = fixed(x, nf());
    if (decimalPoint() != '.') t.replace('.', decimalPoint());
    return t;
}

NumberInput::NumberInput():
    TextInput{onDemand<State>}
{}

NumberInput &NumberInput::associate(Out<NumberInput> self)
{
    return View::associate<NumberInput>(self);
}

double NumberInput::min() const
{
    return me().min();
}

NumberInput &NumberInput::min(double newValue)
{
    me().min(newValue);
    return *this;
}

NumberInput &NumberInput::min(Definition<double> &&f)
{
    me().min(move(f));
    return *this;
}

double NumberInput::max() const
{
    return me().max();
}

NumberInput &NumberInput::max(double newValue)
{
    me().max(newValue);
    return *this;
}

NumberInput &NumberInput::max(Definition<double> &&f)
{
    me().max(move(f));
    return *this;
}

double NumberInput::precision() const
{
    return me().precision();
}

NumberInput &NumberInput::precision(double newValue)
{
    me().precision(newValue);
    return *this;
}

NumberInput &NumberInput::precision(Definition<double> &&f)
{
    me().precision(move(f));
    return *this;
}

char NumberInput::decimalPoint() const
{
    return me().decimalPoint();
}

NumberInput &NumberInput::decimalPoint(char newValue)
{
    me().decimalPoint(newValue);
    return *this;
}

NumberInput &NumberInput::decimalPoint(Definition<char> &&f)
{
    me().decimalPoint(move(f));
    return *this;
}

double NumberInput::value() const
{
    return me().value();
}

NumberInput &NumberInput::value(double newValue)
{
    me().value(newValue);
    return *this;
}

NumberInput &NumberInput::onValueChanged(Fun<void()> &&f)
{
    me().value.onChanged(move(f));
    return *this;
}

NumberInput::State &NumberInput::me()
{
    return View::me().as<State>();
}

const NumberInput::State &NumberInput::me() const
{
    return View::me().as<State>();
}

} // namespace cc
