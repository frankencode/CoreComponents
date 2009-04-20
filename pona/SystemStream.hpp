/*
 * SystemStream.hpp -- a stream associated with a file descriptor
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_SYSTEMSTREAM_HPP
#define PONA_SYSTEMSTREAM_HPP

#include "Stream.hpp"
#include "Time.hpp"

namespace pona
{

class SystemStream: public Stream
{
public:
	SystemStream(int fd);
	~SystemStream();
	
	int fd() const;
	bool interactive() const;
	
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

} // namespace pona

#endif // PONA_SYSTEMSTREAM_HPP

