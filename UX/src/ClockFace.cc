/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ClockFace>
#include <cc/GlyphRun>
#include <cc/Application>
#include <cc/DEBUG>

namespace cc {

struct ClockFace::State: public Control::State
{
    State()
    {
        hourOfDay.restrict([this](int &newValue, int){
            return 0 <= newValue && newValue <= 23;
        });

        minute.restrict([this](int &newValue, int){
            return 0 <= newValue && newValue <= 59;
        });

        second.restrict([this](int &newValue, int){
            return 0 <= newValue && newValue <= 59;
        });

        hourOfDay([this]{
            int value = hour();
            if (pm()) {
                value += 12;
                if (value == 24) value = 0;
            }
            return value;
        });

        paper([this]{ return theme().dialogColor(); });
        size([this]{ return preferredSize(); });

        paint([this]{
            using std::cos, std::sin;

            const double r = [this]{
                const Size m = margin();
                const double w = width() - 2 * m[0];
                const double h = height() - 2 * m[1];
                return ((w < h) ? w : h) / 2;
            }();

            Painter painter{this};

            painter
            .translate(size() / 2)
            .moveTo({r, 0})
            .arcAround({0, 0}, rad(360))
            .setPen(theme().clockDialClockFaceColor(pressed()))
            .fill();

            painter.setPen(theme().clockDialClockFaceTextColor(pressed()));
            const double l = r - thumbRadius();
            for (int i = 0; i < 12; ++i) {
                int hour = i + 3;
                if (hour > 12) hour -= 12;
                double phi = i * rad(30);
                Point p = Point{cos(phi), sin(phi)} * l;
                GlyphRun g{str(hour)};
                Point o = (p + Point{-g.advance()[0], g.maxAscender()} / 2).round();
                painter.showGlyphRun(o, g);
            }

            if (
                mode == Mode::SetHour ||
                mode == Mode::SetMinute ||
                mode == Mode::SetSecond
            ) {
                int currentValue = 0;
                double currentAngle = 0;
                switch (mode) {
                    case Mode::SetHour: {
                        currentValue = hour();
                        currentAngle = hourAngle();
                        break;
                    }
                    case Mode::SetMinute: {
                        currentValue = minute();
                        currentAngle = minuteAngle();
                        break;
                    }
                    case Mode::SetSecond: {
                        currentValue = second();
                        currentAngle = secondAngle();
                        break;
                    }
                    default: ;
                };

                GlyphRun g{str(currentValue)};
                Point p = Point{cos(currentAngle), sin(currentAngle)} * l;
                Point o = (p + Point{-g.advance()[0], g.maxAscender()} / 2).round();
                painter
                .setPen(
                    Pen{theme().clockDialCurrentValueColor(pressed())}
                    .lineWidth(sp(2))
                )
                .moveTo({0, 0})
                .lineTo(p)
                .stroke()
                .moveTo(p + Point{thumbRadius() - sp(3), 0})
                .arcAround(p, rad(360))
                .fill()
                .moveTo({sp(4), 0})
                .arcAround({0, 0}, rad(360))
                .fill()
                .setPen(theme().clockDialCurrentValueTextColor(pressed()))
                .showGlyphRun(o, g);
            }
        });

        onPointerPressed([this](const PointerEvent &event){
            if (mode() == Mode::SetHour) {
                hour(hourFromPointerPos(event.pos()));
            }
            else if (mode() == Mode::SetMinute) {
                dragStart_ = event.pos();
                minute(nearMinuteFromPointerPos(event.pos()));
            }
            else if (mode() == Mode::SetSecond) {
                dragStart_ = event.pos();
                second(nearSecondFromPointerPos(event.pos()));
            }
            return true;
        });

        onPointerReleased([this](const PointerEvent &event){
            if (mode() == Mode::SetMinute) {
                if (!Application{}.pointerIsDragged(event, dragStart_)) {
                    minute(nearMinuteFromPointerPos(event.pos()));
                }
            }
            else if (mode() == Mode::SetSecond) {
                if (!Application{}.pointerIsDragged(event, dragStart_)) {
                    minute(nearSecondFromPointerPos(event.pos()));
                }
            }
            return true;
        });

        onPointerMoved([this](const PointerEvent &event){
            if (!pressed()) return false;
            if (mode() == Mode::SetHour) {
                hour(hourFromPointerPos(event.pos()));
            }
            else if (mode() == Mode::SetMinute) {
                minute(minuteFromPointerPos(event.pos()));
            }
            else if (mode() == Mode::SetSecond) {
                second(secondFromPointerPos(event.pos()));
            }
            return true;
        });
    }

    bool isStatic() const
    {
        return false;
    }

    void setHourOfDay(int newValue)
    {
        if (newValue < 0) newValue = 0;
        else if (newValue > 23) newValue = 23;
        pm(12 < newValue);
        hour(newValue - 12 * pm());
    }

    int hourFromPointerPos(Point pos) const
    {
        Point p = pos - size() / 2;
        double phi = deg(cc::angle(p)) - 270;
        if (phi < 0) phi += 360;
        int hour = std::round(12 * phi / 360);
        if (hour == 0) hour = 12;
        else if (hour <= 1) hour = 1;
        else if (hour > 12) hour = 12;
        return hour;
    }

    int minuteFromPointerPos(Point pos) const
    {
        Point p = pos - size() / 2;
        double phi = deg(cc::angle(p)) - 270;
        if (phi < 0) phi += 360;
        int minute = std::round(60 * phi / 360);
        if (minute <= 0) minute = 0;
        else if (minute >= 59) minute = 59;
        return minute;
    }

    int nearMinuteFromPointerPos(Point pos) const
    {
        int minute = minuteFromPointerPos(pos);
        int carry = minute % 5;
        if (carry == 0);
        else if (carry <= 2) minute -= carry;
        else minute += 5 - carry;
        if (minute < 0) minute += 60;
        else if (minute >= 60) minute -= 60;
        return minute;
    }

    int secondFromPointerPos(Point pos) const
    {
        Point p = pos - size() / 2;
        double phi = deg(cc::angle(p)) - 270;
        if (phi < 0) phi += 360;
        int second = std::round(60 * phi / 360);
        if (second <= 0) second = 0;
        else if (second >= 59) second = 59;
        return second;
    }

    int nearSecondFromPointerPos(Point pos) const
    {
        int second = secondFromPointerPos(pos);
        int carry = second % 5;
        if (carry == 0);
        else if (carry <= 2) second -= carry;
        else second += 5 - carry;
        if (second < 0) second += 60;
        else if (second >= 60) second -= 60;
        return second;
    }

    double hourAngle() const
    {
        double phi = 30 * hour() + 270;
        if (phi >= 360) phi -= 360;
        return rad(phi);
    }

    double minuteAngle() const
    {
        double phi = 6 * minute() + 270;
        if (phi >= 360) phi -= 360;
        return rad(phi);
    }

    double secondAngle() const
    {
        double phi = 6 * second() + 270;
        if (phi >= 360) phi -= 360;
        return rad(phi);
    }

    double thumbRadius() const
    {
        return sp(26);
    }

    Size preferredSize() const
    {
        return 2 * margin() + Size{sp(256), sp(256)};
    }

    Point dragStart_;
    Property<Size> margin { sp(24), sp(24) };
    Property<Mode> mode { Mode::SetHour };
    Property<bool> pm;
    Property<int> hour { 12 };
    Property<int> hourOfDay;
    Property<int> minute { 0 };
    Property<int> second { 0 };
};

ClockFace::ClockFace():
    Control{onDemand<State>}
{}

ClockFace &ClockFace::associate(Out<ClockFace> self)
{
    return View::associate<ClockFace>(self);
}

Size ClockFace::margin() const
{
    return me().margin();
}

ClockFace &ClockFace::margin(Size newValue)
{
    me().margin(newValue);
    return *this;
}

ClockFace &ClockFace::margin(Definition<Size> &&f)
{
    me().margin(move(f));
    return *this;
}

ClockFace::Mode ClockFace::mode() const
{
    return me().mode();
}

ClockFace &ClockFace::mode(Mode newValue)
{
    me().mode(newValue);
    return *this;
}

ClockFace &ClockFace::mode(Definition<Mode> &&f)
{
    me().mode(move(f));
    return *this;
}

bool ClockFace::pm() const
{
    return me().pm();
}

ClockFace &ClockFace::pm(bool newValue)
{
    me().pm(newValue);
    return *this;
}

ClockFace &ClockFace::pm(Definition<bool> &&f)
{
    me().pm(move(f));
    return *this;
}

int ClockFace::hour() const
{
    return me().hourOfDay();
}

ClockFace &ClockFace::hour(int newValue)
{
    me().setHourOfDay(newValue);
    return *this;
}

ClockFace &ClockFace::onHourChanged(Fun<void()> &&f)
{
    me().hourOfDay.onChanged(move(f));
    return *this;
}

int ClockFace::minute() const
{
    return me().minute();
}

ClockFace &ClockFace::minute(int newValue)
{
    while (newValue <= 0) newValue += 60;
    while (newValue >= 60) newValue -= 60;
    me().minute(newValue);
    return *this;
}

ClockFace &ClockFace::onMinuteChanged(Fun<void()> &&f)
{
    me().minute.onChanged(move(f));
    return *this;
}

int ClockFace::second() const
{
    return me().second();
}

ClockFace &ClockFace::second(int newValue)
{
    me().second(newValue);
    return *this;
}

ClockFace &ClockFace::onSecondChanged(Fun<void()> &&f)
{
    me().second.onChanged(move(f));
    return *this;
}

ClockFace::State &ClockFace::me()
{
    return get<State>();
}

const ClockFace::State &ClockFace::me() const
{
    return get<State>();
}

} // namespace cc
