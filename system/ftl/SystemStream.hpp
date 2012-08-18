/*
 * SystemStream.hpp -- a stream associated with a file descriptor
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
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
	inline static Ref<SystemStream, Owner> newInstance(int fd) {
		return new SystemStream(fd);
	}
	~SystemStream();

	int fd() const;
	bool isTeletype() const;

	bool isOpen() const;
	void close();

	bool readyRead(Time timeout);
	bool readyReadOrWrite(Time timeout);

	int readAvail(void *buf, int bufCapa);
	void write(const void *buf, int bufFill);

	void write(Ref<StringList> parts, const char *sep = "");

	inline int readAvail(String s) { return Stream::readAvail(s); }
	inline void write(const char *s) { Stream::write(s); }
	inline void write(String s) { write(s->data(), s->size()); }

	void closeOnExec();

protected:
	SystemStream(int fd);

	int fd_;
	mutable bool isattyCached_, isatty_;
};

} // namespace ftl

#endif // FTL_SYSTEMSTREAM_HPP
