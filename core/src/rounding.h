/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <math.h>

namespace cc {

inline double roundToZero(double x)
{
    double ip; // integral part
    double fp = modf(x, &ip); // fractional part;
    return ip - int(fp < -0.5) + int(0.5 < fp);
}

inline double ceilToInf(double x)
{
    double ip; // integral part
    double fp = modf(x, &ip); // fractional part;
    return ip - int(fp < 0) + int(0 < fp);
}

inline double roundToInf(double x)
{
    double ip; // integral part
    double fp = modf(x, &ip); // fractional part;
    return ip - int(fp <= -0.5) + int(0.5 <= fp);
}

inline double floorToZero(double x)
{
    double ip; // integral part
    modf(x, &ip);
    return ip;
}

inline double floorToMinusInf(double x)
{
    double ip; // integral part
    double fp = modf(x, &ip); // fractional part;
    return ip - int(fp < 0);
}

template<class T>
inline int sign(T x) { return 1 - 2 * (x < 0); }

template<class T>
inline int sgn(T x) { return (0 < x) - (x < 0); }

template<class T>
inline T bound(T min, T x, T max) {
    if (x < min) x = min;
    else if (x > max) x = max;
    return x;
}

/** round x down to the next multiple of a whole number g
  */
template<class G, class X>
X roundDownToNext(G g, X x) { return x - x % g; }

/** round x up to the next multiple of a whole number g
  */
template<class G, class X>
X roundUpToNext(G g, X x) { return x - x % g + (x % g != 0 ? g : 0); }

} // namespace cc
