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
        *h = std::numeric_limits<double>::quiet_NaN();
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

void rgbToHls(double r, double g, double b, double *h, double *l, double *s)
{
    double min, max; {
        auto pair = std::minmax<double>({r, g, b});
        min = pair.first;
        max = pair.second;
    }

    *l = (min + max) / 2.;
    if (min == max) {
        *s = 0.;
        *h = std::numeric_limits<double>::quiet_NaN();
        return;
    }

    double delta = max - min;
    *s = (*l <= 0.5) ? delta / (min + max) : delta / (2. - (min + max));
    if (r == max)
        *h = (g - b) / delta;
    else if (g == max)
        *h = 2. + (b - r) / delta;
    else /* if (b == max) */
        *h = 4. + (r - g) / delta;
    *h *= 60.;
    if (*h < 0.) *h += 360.;
}

void hlsToRgb(double h, double l, double s, double *r, double *g, double *b)
{
    double m2 = (l <= 0.5) ? l * (l + s) : l + s - l * s;
    double m1 = 2. * l - m2;
    if (s == 0.) {
        *r = l;
        *g = l;
        *b = l;
        return;
    }

    struct Util {
        inline static double value(double n1, double n2, double hue)
        {
            if (hue > 360.)
                hue -= 360.;
            else if (hue < 0.)
                hue += 360.;
            if (hue < 60.)
                return n1 + (n2 - n1) * hue / 60.;
            if (hue < 180.)
                return n2;
            if (hue < 240.)
                return n1 + (n2 - n1) * (240. - hue) / 60.;
            return n1;
        }
    };

    *r = Util::value(m1, m2, h + 120.);
    *g = Util::value(m1, m2, h);
    *b = Util::value(m1, m2, h - 120.);
}

} // namespace cc
