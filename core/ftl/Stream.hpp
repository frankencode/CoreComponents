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

#include "string.hpp"

namespace ftl
{

FTL_EXCEPTION(StreamException, Exception);
FTL_EXCEPTION(StreamSemanticException, StreamException);
FTL_EXCEPTION(StreamIoException, StreamException);
FTL_EXCEPTION(EncodingException, StreamIoException);

class Stream: public Instance
{
public:
	virtual ~Stream() {}

	virtual int readAvail(void *buf, int bufCapa) = 0;
	virtual void write(const void *buf, int bufFill) = 0;

	void read(void *buf, int bufFill);
	string readAll();

	inline int readAvail(string s) { return readAvail(s->data(), s->size()); }
	inline void read(string s) { read(s->data(), s->size()); }
	inline string read(int size) { string s(size); read(s); return s; }
	inline void write(const char *s) { write(s, str::len(s)); }
	inline void write(string s) { write(s->data(), s->size()); }
};

} // namespace ftl

#endif // FTL_STREAM_HPP
