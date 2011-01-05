/*
 * Stream.hpp -- generic data stream
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_STREAM_HPP
#define FTL_STREAM_HPP

#include "String.hpp"

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
	
	virtual int readAvail(void* buf, int bufCapa) = 0;
	virtual void write(const void* buf, int bufFill) = 0;
	
	void read(void* buf, int bufFill);
	String readAll();
	
	inline int readAvail(String s) { return readAvail(s->data(), s->size()); }
	inline void read(String s) { read(s->data(), s->size()); }
	inline String read(int size) { String s = String::uninitialized(size); read(s); return s; }
	inline void write(const char* s) { write(s, str::len(s)); }
	inline void write(String s) { write(s->data(), s->size()); }
};

} // namespace ftl

#endif // FTL_STREAM_HPP
