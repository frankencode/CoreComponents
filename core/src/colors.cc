/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <algorithm>
#include <limits>
#include <cmath>
#include <cc/colors>

namespace cc {

void rgbToHsv(double r, double g, double b, double *h, double *s, double *v)
{
    double min, max; {
        auto pair = std::minmax<double>({r, g, b});
        min = pair.first;
        max = pair.second;
    }

    *v = max;
    *s = (max != 0.) ? ((max - min) / max) : 0.;
    if (*s == 0.) {
        *h = 0.;
        return;
    }

    double delta = max - min;
    if (r == max)
        *h = (g - b) / delta;
    else if (g == max)
        *h = 2. + (b - r) / delta;
    else /*if (b == max) */
        *h = 4. + (r - g) / delta;
    *h *= 60.;
    if (*h < 0.) *h += 360.;
}

void hsvToRgb(double h, double s, double v, double *r, double *g, double *b)
{
    if (s == 0.) {
        *r = v;
        *g = v;
        *b = v;
        return;
    }

    double f, p, q, t;
    int i;

    if (h == 360.) h = 0.;
    h /= 60.;
    i = std::floor(h);
    f = h - i;
    p = v * (1. - s);
    q = v * (1. - s * f);
    t = v * (1. - s * (1. - f));

    switch (i) {
        case 0: *r = v; *g = t; *b = p; break;
        case 1: *r = q; *g = v; *b = p; break;
        case 2: *r = p; *g = v; *b = t; break;
        case 3: *r = p; *g = q; *b = v; break;
        case 4: *r = t; *g = p; *b = v; break;
        case 5: *r = v; *g = p; *b = q; break;
    };
}

void rgbToHsl(double r, double g, double b, double *h, double *s, double *l)
{
    double min, max; {
        auto pair = std::minmax<double>({r, g, b});
        min = pair.first;
        max = pair.second;
    }

    *l = (min + max) / 2.;
    if (min == max) {
        *s = 0.;
        *h = 0.;
        return;
    }

    double delta = max - min;
    *s = (*l <= 0.5) ? (delta / (min + max)) : (delta / (2. - (min + max)));
    if (r == max)
        *h = (g - b) / delta;
    else if (g == max)
        *h = 2. + (b - r) / delta;
    else if (b == max)
        *h = 4. + (r - g) / delta;
    *h *= 60.;
    if (*h < 0.) *h += 360.;
}

void hslToRgb(double h, double s, double l, double *r, double *g, double *b)
{
    double c = (1. - std::abs(2. * l - 1.)) * s;
    double q = h / 60.;
    double x = c * (1. - std::abs(std::fmod(q, 2.) - 1.));

    double r1, g1, b1;
    if (q <= 1)      { r1 = c; g1 = x; b1 = 0; }
    else if (q <= 2) { r1 = x; g1 = c; b1 = 0; }
    else if (q <= 3) { r1 = 0; g1 = c; b1 = x; }
    else if (q <= 4) { r1 = 0; g1 = x; b1 = c; }
    else if (q <= 5) { r1 = x; g1 = 0; b1 = c; }
    else             { r1 = c; g1 = 0; b1 = x; }

    double m = l - c / 2.;
    *r = r1 + m;
    *g = g1 + m;
    *b = b1 + m;
}

} // namespace cc
