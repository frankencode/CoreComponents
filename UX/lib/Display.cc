/*
 * Copyright (C) 2020-2024 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/Display>

namespace cc {

struct Display::State final: public Object::State
{
    State(Point pos_, Size size_, Size dpi_, DisplayMode nativeMode_):
        pos{pos_},
        size{size_},
        dpi{dpi_},
        nativeMode{nativeMode_}
    {}

    Property<Point> pos;
    Property<Size> size;
    Property<Size> dpi;
    Property<DisplayMode> nativeMode;

    const Property<Size> dpcm { [this]{ return dpi() / 2.54; } };
    const Property<double> diagonal { [this]{ return (size() / dpi()).abs(); } };
    const Property<bool> isHandheld { [this]{ return diagonal() < 11; } };
};

Display::Display(
    Point pos,
    Size size,
    Size dpi,
    DisplayMode nativeMode
):
    Object{
        new State{
            pos, size, dpi, nativeMode
        }
    }
{}

Point Display::pos() const { return me().pos(); }
Size Display::size() const { return me().size(); }
Rect Display::geometry() const { return Rect{pos(), size()}; }
Size Display::dpi() const { return me().dpi(); }
DisplayMode Display::nativeMode() const { return me().nativeMode(); }
Size Display::dpcm() const { return me().dpcm(); }
double Display::diagonal() const { return me().diagonal(); }
bool Display::isHandheld() const { return me().isHandheld(); }

String Display::toString() const
{
    return Format{}
        << "Display {" << nl
        << "  pos       : " << size() << nl
        << "  size      : " << pos() << nl
        << "  dpi       : " << dpi() << nl
        << "  nativeMode: " << nativeMode() << nl
        << "}";
}

const Display::State &Display::me() const
{
    return Object::me.as<State>();
}

} // namespace cc
