/*
 * Stream.hpp -- generic data stream
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_STREAM_HPP
#define FTL_STREAM_HPP

#include "Array.hpp"

namespace ftl
{

FTL_EXCEPTION(StreamException, Exception);
FTL_EXCEPTION(StreamSemanticException, StreamException);
FTL_EXCEPTION(StreamIoException, StreamException);
FTL_EXCEPTION(StreamEncodingException, StreamIoException);

class Stream: public Instance
{
public:
	virtual ~Stream() {}
	
	virtual int readAvail(void* buf, int bufCapa) = 0;
	virtual void write(const void* buf, int bufFill) = 0;
	
	void read(void* buf, int bufCapa);
	Ref<ByteArray, Owner> readAll();
};

} // namespace ftl

#endif // FTL_STREAM_HPP
