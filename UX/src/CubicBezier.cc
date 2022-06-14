/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/CubicBezier>

namespace cc {

CubicBezier::CubicBezier(Point a, Point b, Point c, Point d):
    a_{a}, b_{b}, c_{c}, d_{d}
{}

Point CubicBezier::operator()(double t) const
{
    double t2 = t * t;
    double t3 = t2 * t;
    return
        (-t3 + 3 * t2 - 3 * t + 1) * a_
        + 3 * (t3 - 2 * t2 + t) * b_
        + 3 * (-t3 + t2) * c_
        + t3 * d_;
}

double CubicBezier::length(long steps) const
{
    if (steps <= 1) return (a_ - b_).abs();

    const auto &f = *this;

    double sum = 0;
    double dt = 1. / steps;
    double t = 0;
    Point a = a_;
    for (long i = 0; i < steps - 1; ++i) {
        t += dt;
        Point b = f(t);
        sum += (a - b).abs();
        a = b;
    }

    sum += (a - d_).abs();

    return sum;
}

} // namespace cc
