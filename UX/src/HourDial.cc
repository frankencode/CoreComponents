/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/HourDial>

namespace cc {

struct HourDial::State final: public ClockDial::State
{
    State():
        ClockDial::State{12}
    {}

    int minValue() const override { return 1; }

    int maxValue() const override { return 12; }

    int faceIndexToValue(int i) const override
    {
        int hour = i + 3;
        if (hour > 12) hour -= 12;
        return hour;
    }

    String faceValueToString(int value) const override
    {
        return str(value);
    }
};

HourDial::HourDial():
    ClockDial{onDemand<State>}
{}

} // namespace cc
