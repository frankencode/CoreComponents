 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */

#include "CircularBuffer.hpp"
#include "LineSource.hpp"

namespace ftl
{

LineSource::LineSource(Stream *stream, const char *eol, int maxLineLength)
	: stream_(stream),
	  eol_(eol),
	  cachedLines_(0),
	  cache_(Cache::create(maxLineLength)),
	  buf_(ByteArray::create(maxLineLength))
{}

Stream *LineSource::stream() const { return stream_; }

bool LineSource::hasMore()
{
	while ((cachedLines_ == 0) && (readAvail()));
	return (cachedLines_ > 0);
}

bool LineSource::read(String *line)
{
	bool more = hasMore();
	if (more) *line = readLine();
	return more;
}

String LineSource::readLine()
{
	if (!hasMore())
		return "";

	int nk = eol_->size();
	int k = 0;
	int i = 0;
	while (k < nk) {
		char ch = cache_->pop();
		k = (ch == eol_->at(k)) ? k + 1 : 0;
		buf_->set(i, ch);
		++i;
	}

	--cachedLines_;

	return String(buf_->data(), i - nk);
}

bool LineSource::readAvail()
{
	int bufFill = stream_->readAvail(buf_->data(), buf_->size());

	for (int i = 0, nk = eol_->size(), k = 0; i < bufFill; ++i)
	{
		if (cache_->fill() == cache_->size())
			FTL_THROW(StreamIoException, str::cat("Maximum line length of ", ftl::intToStr(cache_->size()), " bytes exceeded"));

		char ch = buf_->at(i);
		k = (ch == eol_->at(k)) ? k + 1 : 0;

		if (k == nk) {
			k = 0;
			++cachedLines_;
		}

		cache_->push(ch);
	}

	return bufFill > 0;
}

} // namespace ftl
