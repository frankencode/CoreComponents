/*
 * LineSource.cpp -- canonically buffered data source
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */

#include "LineSource.hpp"

namespace ftl
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

bool LineSource::hasNext()
{
	while ((cachedLines_ == 0) && (readAvail()));
	return cachedLines_ > 0;
}

String LineSource::next()
{
	return readLine();
}

String LineSource::readLine()
{
	if (!hasNext())
		return "";
	
	int nk = eol_->size();
	int k = 0;
	int i = 0;
	while (k < nk) {
		char ch = cache_.pop();
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

bool LineSource::readAvail()
{
	int bufFill = stream_->readAvail(buf_, bufCapa_);
	
	for (int i = 0, nk = eol_->size(), k = 0; i < bufFill; ++i)
	{
		if (cache_.fill() == cache_.size())
			FTL_THROW(StreamIoException, str::cat("Maximum line length of ", ftl::intToStr(cache_.size()), " bytes exceeded"));

		char ch = buf_[i];
		k = (ch == eol_->at(k)) ? k + 1 : 0;
	
		if (k == nk) {
			k = 0;
			++cachedLines_;
		}
		
		cache_.push(ch);
	}
	
	return bufFill > 0;
}

Ref<Stream> LineSource::stream() const
{
	return stream_;
}

} // namespace ftl
