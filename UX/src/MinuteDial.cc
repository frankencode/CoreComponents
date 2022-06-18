/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/MinuteDial>

namespace cc {

struct MinuteDial::State final: public ClockDial::State
{
    State():
        ClockDial::State{0}
    {}

    int minValue() const override { return 0; }

    int maxValue() const override { return 55; }

    int faceIndexToValue(int i) const override
    {
        int hour = i + 3;
        if (hour >= 12) hour -= 12;
        return 5 * hour;
    }

    String faceValueToString(int value) const override
    {
        return dec(value, 2).replaced(" ", "0");
    }
};

MinuteDial::MinuteDial():
    ClockDial{onDemand<State>}
{}

} // namespace cc
