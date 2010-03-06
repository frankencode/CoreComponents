/*
 * LineSource.cpp -- canonically buffered data source
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../LICENSE for the license.
 */

#include "LineSource.hpp"

namespace pona
{

LineSource::LineSource(Ref<Stream> stream, int bufCapa, const char* eol)
	: stream_(stream),
	  eol_(eol),
	  cachedLines_(0),
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
	bool h;
	if (!eoi) eoi = &h;
	*eoi = false;
	
	while ((cachedLines_ == 0) && (!*eoi))
		readAvail(eoi);
	
	if (*eoi)
		return String();
	
	int nk = eol_->size();
	int k = 0;
	int i = 0;
	while (k < nk)
	{
		char ch = cache_.popFront();
		k = (ch == eol_->at(k)) ? k + 1 : 0;
		buf_[i] = ch;
		++i;
	}

	--cachedLines_;
	
	return String(buf_, i - nk);
}

int LineSource::cachedLines() const
{
	return cachedLines_;
}

void LineSource::readAvail(bool* eoi)
{
	if (eoi) *eoi = false;
	
	int bufFill = stream_->readAvail(buf_, bufCapa_);
	
	if (bufFill == 0) {
		if (eoi) *eoi = true;
		return;
	}
	
	for (int i = 0, nk = eol_->size(), k = 0; i < bufFill; ++i)
	{
		if (cache_.fill() == cache_.size())
			PONA_THROW(StreamIoException, pona::strcat("Maximum line length of ", pona::intToStr(cache_.size()), " bytes exceeded"));

		char ch = buf_[i];
		k = (ch == eol_->at(k)) ? k + 1 : 0;
	
		if (k == nk) {
			k = 0;
			++cachedLines_;
		}
		
		cache_.pushBack(ch);
	}
}

Ref<Stream> LineSource::stream() const
{
	return stream_;
}

} // namespace pona
