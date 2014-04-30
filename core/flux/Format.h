/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_FORMAT_H
#define FLUX_FORMAT_H

#include "String.h"
#include "Variant.h"
#include "str.h"

namespace flux
{

class FormatSignal {};

extern FormatSignal nl;
extern FormatSignal flush;

class Stream;
template<class T> class Queue;

class Format: public Ref<StringList>
{
public:
	typedef Ref<StringList> Super;

	Format(String pattern, Stream *stream = 0);
	Format(Stream *stream = 0);
	~Format();

	Format(const Format &b);
	Format &operator=(const Format &b);

	Format &operator<<(const String &s);

	Format &operator<<(const FormatSignal &s);

	inline Format &operator<<(const ByteArray *s) {
		if (isNull_) return *this;
		return *this << String(s);
	}

	template<class T>
	inline Format &operator<<(const T &x) {
		if (isNull_) return *this;
		return *this << str(x);
	}

	inline Format &operator<<(const Ref<StringList> &x) {
		if (isNull_) return *this;
		return *this << x->join("");
	}

private:
	void flush();

	Ref<Stream> stream_;
	bool isNull_;
	Ref< Queue<int> > placeHolder_;
};

class NullFormat {
public:
	template<class T>
	inline const NullFormat &operator<<(T) const { return *this; }
};

} // namespace flux

#endif // FLUX_FORMAT_H
