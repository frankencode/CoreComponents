/*
 * Stream.hpp -- fundamental representation of a data stream
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_STREAM_HPP
#define PONA_STREAM_HPP

#include "atoms"

namespace pona
{

PONA_EXCEPTION(StreamException, Exception);
PONA_EXCEPTION(StreamSemanticException, StreamException);
PONA_EXCEPTION(StreamIoException, StreamException);
PONA_EXCEPTION(StreamEncodingException, StreamIoException);

class Stream: public Instance
{
public:
	virtual ~Stream() {}
	
	virtual bool isOpen() const = 0;
	virtual void close() = 0;
	
	virtual int readAvail(void* buf, int bufCapa) = 0;
	virtual void write(const void* buf, int bufFill) = 0;
	
	void read(void* buf, int bufCapa);
};

} // namespace pona

#endif // PONA_STREAM_HPP
