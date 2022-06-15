/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Point>
#include <limits>

namespace cc {

template class Vector<double, 2>;

/** Get positive angle of vector \a v
  * \return Angle in range [0, 2*PI)
  */
double angle(Point v)
{
    using std::numbers::pi;

    if (v[0] == 0) {
        if (v[1] == 0) return std::numeric_limits<double>::quiet_NaN();
        if (v[1] > 0) return pi/2;
        return 3 * pi / 2;
    }
    else if (v[1] == 0) {
        if (v[0] > 0) return 0;
        return pi;
    }
    double alpha = std::atan(v[1]/v[0]);
    if (v[0] < 0) alpha += pi;
    else if (v[1] < 0) alpha += 2 * pi;
    return alpha;
}

double abs(Point v)
{
    if (v[0] == 0) return std::abs(v[1]);
    else if (v[1] == 0) return std::abs(v[0]);
    return v.abs();
}

} // namespace cc
