/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include "stdio"
#include "Queue"
#include "NullStream"
#include "Format"

namespace flux
{

FormatSignal nl;
FormatSignal flush;

Format::Format(String pattern, Stream *stream)
	: stream_(stream),
	  isNull_(stream && nullStream() ? stream == nullStream() : false)
{
	if (isNull_) return;
	set(StringList::create());
	int i0 = 0, n = 0;
	while (true) {
		int i1 = pattern->find("%%", i0);
		if (i0 < i1)
			get()->append(pattern->copy(i0, i1));
		if (i1 == pattern->count()) break;
		int j = get()->count() + n;
		if (!placeHolder_) placeHolder_ = Queue<int>::create();
		placeHolder_->push(j);
		++n;
		i0 = i1 + 2;
	}
}

Format::Format(Stream *stream)
	: stream_(stream),
	  isNull_(stream && nullStream() ? stream == nullStream() : false)
{
	set(StringList::create());
}

Format::~Format()
{
	flush();
}

void Format::flush()
{
	if (isNull_) return;
	if (stream_ && get()->count() > 0) {
		stream_->write(get());
		get()->clear();
	}
}

Format::Format(const Format &b)
	: Super(b.get()),
	  stream_(b.stream_),
	  isNull_(b.isNull_),
	  placeHolder_(b.placeHolder_)
{}

Format &Format::operator=(const Format &b)
{
	set(b.get());
	stream_ = b.stream_;
	placeHolder_ = b.placeHolder_;
	isNull_ = b.isNull_;
	return *this;
}

Format &Format::operator<<(const String &s)
{
	if (isNull_) return *this;
	int j = get()->count();
	if (placeHolder_) {
		if (placeHolder_->count() > 0)
			j = placeHolder_->pop();
	}
	get()->insert(j, s);
	return *this;
}

Format &Format::operator<<(const FormatSignal &s)
{
	if (isNull_) return *this;
	if (&s == &flux::nl) operator<<(String("\n"));
	else if (&s == &flux::flush) Format::flush();
	return *this;
}

} // namespace flux
