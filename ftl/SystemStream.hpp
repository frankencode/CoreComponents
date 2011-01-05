/*
 * SystemStream.hpp -- a stream associated with a file descriptor
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_SYSTEMSTREAM_HPP
#define FTL_SYSTEMSTREAM_HPP

#include "atoms"
#include "Stream.hpp"
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
	
	inline int readAvail(String s) { return Stream::readAvail(s); }
	inline void write(const char* s) { Stream::write(s); }
	inline void write(String s) { write(s->data(), s->size()); }
	
	void closeOnExec();
	
	bool continueOnInterrupt() const;
	void setContinueOnInterrupt(bool on);
	
protected:
	int fd_;
	mutable bool isattyCached_, isatty_;
	bool continueOnInterrupt_;
};

} // namespace ftl

#endif // FTL_SYSTEMSTREAM_HPP
