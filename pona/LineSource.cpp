/*
 * LineSource.cpp -- canonically buffered data source
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */

#include "LineSource.hpp"

namespace pona
{

LineSource::LineSource(Ref<Stream> stream, int bufCapa, String eol)
	: stream_(stream),
	  eol_(eol),
	  cacheFillLines_(0),
	  cache_(bufCapa),
	  bufCapa_(bufCapa),
	  buf_(new char[bufCapa])
{}

LineSource::~LineSource()
{
	delete[] buf_;
	buf_ = 0;
}

String LineSource::readLine(bool* eoi)
{
	if (eoi) *eoi = false;
	
	while (cacheFillLines_ == 0)
	{
		int bufFill = stream_->readAvail(buf_, bufCapa_);
		
		if (bufFill == 0) {
			if (eoi) *eoi = true;
			return String();
		}
		
		for (int i = 0, nk = eol_->length(), k = 0; i < bufFill; ++i)
		{
			if (cache_.fill() == cache_.size())
				PONA_THROW(StreamIoException, "Input buffer exhausted");

			Char ch = buf_[i];
			k = (ch == eol_->get(k)) ? k + 1 : 0;
		
			if (k == nk)
			{
				k = 0;
				++cacheFillLines_;
			}
			
			cache_.pushBack(ch);
		}
	}

	int nk = eol_->length();
	int k = 0;
	int i = 0;
	while (k < nk)
	{
		Char ch = cache_.popFront();
		k = (ch == eol_->get(k)) ? k + 1 : 0;
		buf_[i] = ch;
		++i;
	}

	--cacheFillLines_;
	
	return String(buf_, i - nk);
}

Ref<Stream> LineSource::stream() const
{
	return stream_;
}

} // namespace pona
