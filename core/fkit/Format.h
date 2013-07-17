/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FKIT_FORMAT_H
#define FKIT_FORMAT_H

#include "String.h"
#include "Variant.h"
#include "str.h"

namespace fkit
{

class FormatSignal {};

extern FormatSignal nl;
extern FormatSignal flush;

FKIT_EXCEPTION(FormatException, Exception);

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

	template<class T>
	inline Format &operator<<(const T &x) { return *this << str(x); }

	inline Format &operator<<(const Ref<StringList> &x) { return *this << x->join(""); }


private:
	void flush();

	Ref<Stream> stream_;
	Ref< Queue<int> > placeHolder_;
};

} // namespace fkit

#endif // FKIT_FORMAT_H
