/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/HourDial>
#include <cc/GlyphRun>
#include <cc/DEBUG>

namespace cc {

struct HourDial::State final: public Control::State
{
    State()
    {
        value.restrict([](int &newValue, int){
            return 1 <= newValue && newValue <= 12;
        });

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
            .setPen(theme().hourDialClockFaceColor(pressed()))
            .fill();

            painter.setPen(theme().hourDialClockFaceTextColor(pressed()));
            const double l = r - thumbRadius();
            GlyphRun h;
            Point u, v;
            for (int i = 0; i < 12; ++i) {
                int hour = 3 + i;
                if (hour > 12) hour -= 12;
                double phi = i * rad(30);
                Point p = Point{cos(phi), sin(phi)} * l;
                GlyphRun g{str(hour)};
                Point o = p + Point{-g.advance()[0], g.maxAscender()} / 2;
                if (hour == value()) {
                    h = g;
                    u = p;
                    v = o;
                }
                else
                    painter.showGlyphRun(o, g);
            }

            painter
            .setPen(
                Pen{theme().hourDialCurrentHourColor(pressed())}
                .lineWidth(sp(2))
            )
            .moveTo({0, 0})
            .lineTo(u)
            .stroke()
            .moveTo(u + Point{thumbRadius() - sp(3), 0})
            .arcAround(u, rad(360))
            .fill()
            .moveTo({sp(4), 0})
            .arcAround({0, 0}, rad(360))
            .fill()
            .setPen(theme().hourDialCurrentHourTextColor(pressed()))
            .showGlyphRun(v, h);
        });

        size([this]{ return preferredSize(); });

        onPointerPressed([this](const PointerEvent &event){
            selectHour(event.pos());
            return true;
        });

        onPointerMoved([this](const PointerEvent &event){
            selectHour(event.pos());
            return true;
        });
    }

    void selectHour(Point pos)
    {
        Point p = pos - size() / 2;
        double phi = cc::angle(p);
        int hour = 3 + static_cast<int>(std::floor(deg(phi)) + 15) / 30;
        if (hour > 12) hour -= 12;
        value(hour);
    }

    double thumbRadius() const { return sp(28); }

    Size preferredSize() const override
    {
        return 2 * margin() + Size{sp(256), sp(256)};
    }

    Property<Size> margin { sp(24), sp(24) };
    Property<int> value { 12 };
};

HourDial::HourDial():
    Control{onDemand<State>}
{}

Size HourDial::margin() const
{
    return me().margin();
}

HourDial &HourDial::margin(Size newValue)
{
    me().margin(newValue);
    return *this;
}

HourDial &HourDial::margin(Definition<Size> &&f)
{
    me().margin(move(f));
    return *this;
}

int HourDial::value() const
{
    return me().value();
}

HourDial &HourDial::value(int newValue)
{
    me().value(newValue);
    return *this;
}

HourDial::State &HourDial::me()
{
    return get<State>();
}

const HourDial::State &HourDial::me() const
{
    return get<State>();
}

} // namespace cc
