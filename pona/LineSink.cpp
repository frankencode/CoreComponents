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

#include "LineSink.hpp"

namespace pona
{

LineSink::LineSink(Ref<Stream> stream, int bufCapa, String eol)
	: stream_(stream),
	  eol_(eol),
	  bufCapa_(bufCapa),
	  buf_(new uint8_t[bufCapa])
{}

LineSink::~LineSink()
{
	delete[] buf_;
	buf_ = 0;
}

void LineSink::writeLine(String line)
{
	int n = line->length();
	int n2 = eol_->length();

	if (n + n2 > bufCapa_)
		PONA_THROW(StreamIoException, "Output buffer exhausted");

	for (int i = 0; i < n; ++i)
		buf_[i] = line->get(i);

	for (int i = 0; i < n2; ++i)
		buf_[n + i] = eol_->get(i);

	stream_->write(buf_, n + n2);
}

void LineSink::write(String buf)
{
	CString utf8 = buf.utf8();
	stream_->write(utf8, utf8->length() - 1);
}

Ref<Stream> LineSink::stream() const
{
	return stream_;
}

} // namespace pona
