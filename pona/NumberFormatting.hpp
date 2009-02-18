/****************************************************************************
**
** This file is part of libPONA - The Portable Network Abstraction Library.
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

#ifndef PONA_NUMBERFORMATTING_HPP
#define PONA_NUMBERFORMATTING_HPP

#include "String.hpp"

namespace pona
{

PONA_EXCEPTION(FormattingException, Exception);

bool parseBool(String text, int index = 0, int* length = 0);

String printInteger(uint64_t value, int sign = 1, int base = 10, int width = 0);
void parseInteger(uint64_t* value, int* sign, String text, int index = 0, int* length = 0);

String printFloatingPoint(float64_t x, int base = 10, int wi = 0, int wf = 0, int w = 0, char fill = ' ');
float64_t parseFloatingPoint(String text, int index = 0, int* length = 0);

} // namespace pona

#endif //  PONA_NUMBERFORMATTING_HPP
