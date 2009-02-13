/****************************************************************************
**
** This file is part of libPONA - The Portable Network Abstractions Library.
**
** Copyright (C) 2007-2009  Frank Mertens
**
** This file is part of a free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License as published
** by the Free Software Foundation, either version 3 of the License,
** or (at your option) any later version.
**
** The library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this libary.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#ifndef PONA_ROUNDING_HPP
#define PONA_ROUNDING_HPP

#include <math.h>

namespace pona
{

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
	return ip - int(fp < 0) + int(fp > 0);
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

} // namespace pona

#endif // PONA_ROUNDING_HPP
