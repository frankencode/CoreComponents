/*
 * SystemStream.hpp -- a stream associated with a file descriptor
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef FTL_SYSTEMSTREAM_HPP
#define FTL_SYSTEMSTREAM_HPP

#include "atoms"
#include "Time.hpp"

namespace ftl
{

class SystemStream: public Stream
{
public:
	SystemStream(int fd);
	~SystemStream();
	
	int fd() const;
	bool isTeletype() const;
	
	bool isOpen() const;
	void close();
	
	bool readyRead(Time timeout);
	bool readyReadOrWrite(Time timeout);
	
	int readAvail(void* buf, int bufCapa);
	void write(const void* buf, int bufFill);
	
	void closeOnExec();
	
protected:
	int fd_;
	mutable bool isattyCached_, isatty_;
};

} // namespace ftl

#endif // FTL_SYSTEMSTREAM_HPP
