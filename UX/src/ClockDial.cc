/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ClockDial>
#include <cc/GlyphRun>
#include <cc/DEBUG>

namespace cc {

ClockDial::State::State(int initialValue):
    value{initialValue}
{}

void ClockDial::State::settled()
{
    size([this]{ return preferredSize(); });

    value.restrict([this](int &newValue, int){
        int d = maxValue() - minValue();
        if (d > 12) {
            int g = d / 11;
            newValue -= newValue % g;
        }
        return minValue() <= newValue && newValue <= maxValue();
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
        .setPen(theme().clockDialClockFaceColor(pressed()))
        .fill();

        painter.setPen(theme().clockDialClockFaceTextColor(pressed()));
        const double l = r - thumbRadius();
        GlyphRun h;
        Point u, v;
        for (int i = 0; i < 12; ++i) {
            int faceValue = faceIndexToValue(i);
            double phi = i * rad(30);
            Point p = Point{cos(phi), sin(phi)} * l;
            GlyphRun g{faceValueToString(faceValue)};
            Point o = p + Point{-g.advance()[0], g.maxAscender()} / 2;
            if (faceValue == value()) {
                h = g;
                u = p;
                v = o;
            }
            else
                painter.showGlyphRun(o, g);
        }

        painter
        .setPen(
            Pen{theme().clockDialCurrentValueColor(pressed())}
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
        .setPen(theme().clockDialCurrentValueTextColor(pressed()))
        .showGlyphRun(v, h);
    });

    onPointerPressed([this](const PointerEvent &event){
        selectValue(event.pos());
        return true;
    });

    onPointerMoved([this](const PointerEvent &event){
        selectValue(event.pos());
        return true;
    });
}

void ClockDial::State::selectValue(Point pos)
{
    Point p = pos - size() / 2;
    double phi = cc::angle(p);
    int i = static_cast<int>(std::floor(deg(phi)) + 15) / 30;
    value(faceIndexToValue(i));
}

double ClockDial::State::thumbRadius() const
{
    return sp(28);
}

Size ClockDial::State::preferredSize() const
{
    return 2 * margin() + Size{sp(256), sp(256)};
}

Size ClockDial::margin() const
{
    return me().margin();
}

ClockDial &ClockDial::margin(Size newValue)
{
    me().margin(newValue);
    return *this;
}

ClockDial &ClockDial::margin(Definition<Size> &&f)
{
    me().margin(move(f));
    return *this;
}

int ClockDial::value() const
{
    return me().value();
}

ClockDial &ClockDial::value(int newValue)
{
    me().value(newValue);
    return *this;
}

int ClockDial::minValue() const
{
    return me().minValue();
}

int ClockDial::maxValue() const
{
    return me().maxValue();
}

ClockDial::State &ClockDial::me()
{
    return get<State>();
}

const ClockDial::State &ClockDial::me() const
{
    return get<State>();
}

} // namespace cc
