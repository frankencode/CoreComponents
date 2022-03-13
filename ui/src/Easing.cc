/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/Easing>

namespace cc::ui {

double Easing::Linear(double t)
{
    return t;
}

double Easing::InQuad(double t)
{
    return t * t;
}

double Easing::OutQuad(double t)
{
    return (2 - t) * t;
}

double Easing::InOutQuad(double t)
{
    if (t <= 0.5) return 2 * t * t;
    return -2 * t * t + 4 * t - 1;
}

double Easing::InCubic(double t)
{
    return t * t * t;
}

double Easing::OutCubic(double t)
{
    double h = t - 1;
    return h * h * h + 1;
}

double Easing::InOutCubic(double t)
{
    if (t <= 0.5) return 4 * t * t * t;

    double h = 2 * t - 2;
    return 0.5 * h * h * h + 1;
}

double Easing::InQuart(double t)
{
    return t * t * t * t;
}

double Easing::OutQuart(double t)
{
    double h = t - 1;
    return h * h * h * (1 - t) + 1;
}

double Easing::InOutQuart(double t)
{
    if (t <= 0.5) return 8 * t * t * t * t;

    double h = t - 1;
    return -8 * h * h * h * h + 1;
}

double Easing::InQuint(double t)
{
    return t * t * t * t * t;
}

double Easing::OutQuint(double t)
{
    double h = t - 1;
    return h * h * h * h * h + 1;
}

double Easing::InOutQuint(double t)
{
    if (t <= 0.5) return 16 * t * t * t * t * t;

    double h = 2 * t - 2;
    return 0.5 * h * h * h * h * h + 1;
}

double Easing::InSine(double t)
{
    return std::sin((t - 1) * M_PI_2) + 1;
}

double Easing::OutSine(double t)
{
    return std::sin(t * M_PI_2);
}

double Easing::InOutSine(double t)
{
    return 0.5 * (1 - std::cos(t * M_PI));
}

double Easing::InCirc(double t)
{
    return 1 - std::sqrt(1 - t * t);
}

double Easing::OutCirc(double t)
{
    return std::sqrt((2 - t) * t);
}

double Easing::InOutCirc(double t)
{
    if (t <= 0.5) return 0.5 * (1 - std::sqrt(1 - 4 * t * t));
    return 0.5 * std::sqrt(-(2 * t - 3) * (2 * t - 1)) + 0.5;
}

double Easing::InExpo(double t)
{
    return (t != 0) ? std::pow(2, 10 * (t - 1)) : t;
}

double Easing::OutExpo(double t)
{
    return (t != 1) ? 1 - std::pow(2, -10 * t) : t;
}

double Easing::InOutExpo(double t)
{
    if (t == 0 || t == 1) return t;
    if (t <= 0.5) return 0.5 * std::pow(2, 20 * t - 10);
    return -0.5 * std::pow(2, -20 * t + 10) + 1;
}

double Easing::InElastic(double t)
{
    return std::sin(13 * M_PI_2 * t) * std::pow(2, 10 * (t - 1));
}

double Easing::OutElastic(double t)
{
    return std::sin(-13 * M_PI_2 * (t + 1)) * std::pow(2, -10 * t) + 1;
}

double Easing::InOutElastic(double t)
{
    if (t <= 0.5) return 0.5 * std::sin(26 * M_PI_2 * t) * std::pow(2, 20 * t - 10);
    return 0.5 * std::sin(-26 * M_PI_2 * t) * std::pow(2, -20 * t + 10) + 1;
}

double Easing::InBack(double t)
{
    return t * t * t - t * std::sin(M_PI * t);
}

double Easing::OutBack(double t)
{
    double h = 1 - t;
    return 1 - h * h * h - h * std::sin(M_PI * h);
}

double Easing::InOutBack(double t)
{
    if (t <= 0.5) {
        double h = 2 * t;
        return 0.5 * h * h * h - h * std::sin(M_PI * h);
    }

    double h = 2 - 2 * t;
    return 0.5 * (1 - h * h * h - h * std::sin(M_PI * h)) + 0.5;
}

double Easing::InBounce(double t)
{
    return 1 - OutBounce(1 - t);
}

double Easing::OutBounce(double t)
{
    if (t <= 4 / 11.) return 121 / 16. * t * t;
    else if (t <= 8 / 11.) return 363 / 40. * t * t - 99 / 10. * t + 17 / 5.;
    else if (t <= 9 / 10.) return 4356 / 361. * t * t - 35442 / 1805. * t + 16061 / 1805.;
    return 54 / 5. * t * t - 513 / 25. * t + 268 / 25.;
}

double Easing::InOutBounce(double t)
{
    if (t <= 0.5) return 0.5 * InBounce(2 * t);
    return 0.5 * OutBounce(2 * t - 1) + 0.5;
}

double Easing::Bezier::value(double t) const
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
        Point pm = point(sm);
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

Point Easing::Bezier::point(double s) const
{
    const double s2 = s * s;
    const double s3 = s2 * s;
    return 3 * (s3 - 2 * s2 + s) * p1_ + 3 * (-s3 + s2) * p2_ + s3 * Point{1., 1.};
}

} // namespace cc::ui
