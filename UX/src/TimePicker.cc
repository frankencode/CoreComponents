/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/TimePicker>
#include <cc/NumberCell>
#include <cc/Checkbox>
#include <cc/Label>

namespace cc {

struct TimePicker::State final: public View::State
{
    State(int initialHour = 0, int initialMinute = 0):
        hour{initialHour},
        minute{initialMinute}
    {
        font([this]{ return style().defaultFont() * 2.; });

        add(
            NumberCell{}
            .associate(&hourInput_)
            .font([this]{ return font(); })
            .digits(2)
            .min(0)
            .max(23)
            .value(initialHour)
            .attach([this]{
                hour(static_cast<int>(hourInput_.value()));
            })
        );

        add(
            Label{":"}
            .associate(&colonLabel_)
            .font([this]{ return font(); })
            .centerLeft([this]{
                return hourInput_.centerRight();
            })
        );

        add(
            NumberCell{}
            .associate(&minuteInput_)
            .font([this]{ return font(); })
            .digits(2)
            .min(0)
            .max(59)
            .value(initialMinute)
            .topLeft([this]{
                return Point{colonLabel_.right(), hourInput_.top()};
            })
            .attach([this]{
                minute(static_cast<int>(minuteInput_.value()));
            })
        );
    }

    NumberCell hourInput_;
    NumberCell minuteInput_;
    Label colonLabel_;
    Property<Font> font;
    Property<int> hour;
    Property<int> minute;
};

TimePicker::TimePicker():
    View{onDemand<State>}
{}

TimePicker::TimePicker(int initialHour, int initialMinute):
    View{new State{initialHour, initialMinute}}
{}

TimePicker &TimePicker::hour(double newValue)
{
    me().hour(newValue);
    return *this;
}

int TimePicker::hour() const
{
    return me().hour();
}

TimePicker &TimePicker::minute(double newValue)
{
    me().minute(newValue);
    return *this;
}

int TimePicker::minute() const
{
    return me().minute();
}

TimePicker::State &TimePicker::me()
{
    return get<State>();
}

const TimePicker::State &TimePicker::me() const
{
    return get<State>();
}

} // namespace cc
