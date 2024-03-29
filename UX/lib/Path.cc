/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/Path>
#include <cc/CubicBezier>

namespace cc {

double Path::Curve::State::length() const
{
    return CubicBezier{a_, b_, c_, d_}.length();
}

Path::Arc::State::State(Point a, Point b, double alpha):
    a_{a}, b_{b}, alpha_{alpha}
{
    Point p = a - b_;
    alpha0_ = angle(p);
    alpha1_ = alpha0_ + alpha;
    r_ = abs(p);

    c_ = Point{
        std::cos(alpha1_),
        std::sin(alpha1_)
    } * r_;
}

double Path::Arc::State::length() const
{
    return std::abs(r_ * alpha_);
}

} // namespace cc
