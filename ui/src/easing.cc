/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cmath>
#include <cc/ui/easing>

namespace cc {
namespace ui {
namespace easing {

double linear(double t)
{
    return t;
}

double inQuad(double t)
{
    return t * t;
}

double outQuad(double t)
{
    return (2 - t) * t;
}

double inOutQuad(double t)
{
    if (t <= 0.5) return 2 * t * t;
    return -2 * t * t + 4 * t - 1;
}

double inCubic(double t)
{
    return t * t * t;
}

double outCubic(double t)
{
    double h = t - 1;
    return h * h * h + 1;
}

double inOutCubic(double t)
{
    if (t <= 0.5) return 4 * t * t * t;

    double h = 2 * t - 2;
    return 0.5 * h * h * h + 1;
}

double inQuart(double t)
{
    return t * t * t * t;
}

double outQuart(double t)
{
    double h = t - 1;
    return h * h * h * (1 - t) + 1;
}

double inOutQuart(double t)
{
    if (t <= 0.5) return 8 * t * t * t * t;

    double h = t - 1;
    return -8 * h * h * h * h + 1;
}

double inQuint(double t)
{
    return t * t * t * t * t;
}

double outQuint(double t)
{
    double h = t - 1;
    return h * h * h * h * h + 1;
}

double inOutQuint(double t)
{
    if (t <= 0.5) return 16 * t * t * t * t * t;

    double h = 2 * t - 2;
    return 0.5 * h * h * h * h * h + 1;
}

double inSine(double t)
{
    return std::sin((t - 1) * M_PI_2) + 1;
}

double outSine(double t)
{
    return std::sin(t * M_PI_2);
}

double inOutSine(double t)
{
    return 0.5 * (1 - std::cos(t * M_PI));
}

double inCirc(double t)
{
    return 1 - std::sqrt(1 - t * t);
}

double outCirc(double t)
{
    return std::sqrt((2 - t) * t);
}

double inOutCirc(double t)
{
    if (t <= 0.5) return 0.5 * (1 - std::sqrt(1 - 4 * t * t));
    return 0.5 * std::sqrt(-(2 * t - 3) * (2 * t - 1)) + 0.5;
}

double inExpo(double t)
{
    return (t != 0) ? std::pow(2, 10 * (t - 1)) : t;
}

double outExpo(double t)
{
    return (t != 1) ? 1 - std::pow(2, -10 * t) : t;
}

double inOutExpo(double t)
{
    if (t == 0 || t == 1) return t;
    if (t <= 0.5) return 0.5 * std::pow(2, 20 * t - 10);
    return -0.5 * std::pow(2, -20 * t + 10) + 1;
}

double inElastic(double t)
{
    return std::sin(13 * M_PI_2 * t) * std::pow(2, 10 * (t - 1));
}

double outElastic(double t)
{
    return std::sin(-13 * M_PI_2 * (t + 1)) * std::pow(2, -10 * t) + 1;
}

double inOutElastic(double t)
{
    if (t <= 0.5) return 0.5 * std::sin(26 * M_PI_2 * t) * std::pow(2, 20 * t - 10);
    return 0.5 * std::sin(-26 * M_PI_2 * t) * std::pow(2, -20 * t + 10) + 1;
}

double inBack(double t)
{
    return t * t * t - t * std::sin(M_PI * t);
}

double outBack(double t)
{
    double h = 1 - t;
    return 1 - h * h * h - h * std::sin(M_PI * h);
}

double inOutBack(double t)
{
    if (t <= 0.5) {
        double h = 2 * t;
        return 0.5 * h * h * h - h * std::sin(M_PI * h);
    }

    double h = 2 - 2 * t;
    return 0.5 * (1 - h * h * h - h * std::sin(M_PI * h)) + 0.5;
}

double inBounce(double t)
{
    return 1 - outBounce(1 - t);
}

double outBounce(double t)
{
    if (t <= 4 / 11.) return 121 / 16. * t * t;
    else if (t <= 8 / 11.) return 363 / 40. * t * t - 99 / 10. * t + 17 / 5.;
    else if (t <= 9 / 10.) return 4356 / 361. * t * t - 35442 / 1805. * t + 16061 / 1805.;
    return 54 / 5. * t * t - 513 / 25. * t + 268 / 25.;
}

double inOutBounce(double t)
{
    if (t <= 0.5) return 0.5 * inBounce(2 * t);
    return 0.5 * outBounce(2 * t - 1) + 0.5;
}

double Bezier::Instance::getValue(double t) const
{
    if (t == 0) return 0;
    if (t == 1) return 1;

    const int n = 7;
    Point pa { 0., 0. };
    Point pb { 1., 1. };
    double sa = 0;
    double sb = 1;
    for (int i = 0; i < n; ++i) {
        const double sm = 0.5 * (sa + sb);
        Point pm = getPoint(sm);
        if (t <= pm[0]) {
            pb = pm;
            sb = sm;
        }
        else {
            pa = pm;
            sa = sm;
        }
    }

    const double r = (t - pa[0]) / (pb[0] - pa[0]);
    return (1 - r) * pa[1] + r * pb[1];
}

Point Bezier::Instance::getPoint(double s) const
{
    const double s2 = s * s;
    const double s3 = s2 * s;
    return 3 * (s3 - 2 * s2 + s) * p1_ + 3 * (-s3 + s2) * p2_ + s3 * Point{1., 1.};
}

}}} // namespace cc::ui::easing
