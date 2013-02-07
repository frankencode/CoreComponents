 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */
#ifndef FTL_STREAM_HPP
#define FTL_STREAM_HPP

#include "String.hpp"

namespace ftl
{

FTL_EXCEPTION(StreamIoException, Exception);
FTL_EXCEPTION(EncodingException, StreamIoException);

class Stream: public Instance
{
public:
	virtual ~Stream() {}

	virtual int readAvail(void *buf, int bufCapa) = 0;
	virtual void write(const void *buf, int bufFill) = 0;

	void read(void *buf, int bufFill);
	String readAll();

	inline int readAvail(String s) { return readAvail(s->data(), s->size()); }
	inline void read(String s) { read(s->data(), s->size()); }
	inline String read(int size) { String s(size); read(s); return s; }
	inline void write(const char *s) { write(s, str::len(s)); }
	inline void write(String s) { write(s->data(), s->size()); }
};

} // namespace ftl

#endif // FTL_STREAM_HPP
