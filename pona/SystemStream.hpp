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

namespace pona
{

class SystemStream: public Stream
{
public:
	SystemStream(int fd);
	~SystemStream();
	inline int fd() const { return fd_; }
	
	bool isOpen() const;
	void close();

	int readAvail(void* buf, int bufCapa);
	void write(const void* buf, int bufFill);

	bool interactive() const { return isatty_; }
	
protected:
	bool isatty_;
	int fd_;
};

} // namespace pona

#endif // PONA_SYSTEMSTREAM_HPP

