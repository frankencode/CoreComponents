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

#ifndef PONA_LINESINK_HPP
#define PONA_LINESINK_HPP

#include "atoms"
#include "options.hpp"
#include "Stream.hpp"
#include "String.hpp"

namespace pona
{

class LineSink: public Instance
{
public:
	LineSink(Ref<Stream> stream, int bufCapa = PONA_DEFAULT_BUF_CAPA, String eol = "\n");
	~LineSink();

	void writeLine(String line);
	void write(String buf);

	Ref<Stream> stream() const;

private:
	Ref<Stream, Owner> stream_;
	String eol_;
	int bufCapa_;
	uint8_t* buf_;
};

} // namespace pona

#endif // PONA_LINESINK_HPP
