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

#ifndef PONA_RANDOM_HPP
#define PONA_RANDOM_HPP

#include "Clock.hpp"

namespace pona
{

/** Random number generator as described by Lewis et al/1969
  * for the System/360. The generator is reentrant.
  */
class Random
{
public:
	Random(int seed = getTime().miliSeconds())
		: m_((1u << 31) - 1),
		  x_(seed)
	{
		if (x_ == 0)
			x_ = 1;
	}

	
	inline int max() const { return m_ - 1; }
	inline int period() const { return m_ - 1; }
	
	inline int next() {
		x_ = (16807 * x_) % m_; /* 7^5 == 16807 */
		return x_;
	}
	
private:
	unsigned m_;
	unsigned x_;
};

} // namespace pona

#endif // PONA_RANDOM_HPP
