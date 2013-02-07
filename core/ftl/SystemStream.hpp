 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */
#ifndef FTL_SYSTEMSTREAM_HPP
#define FTL_SYSTEMSTREAM_HPP

#include "Stream.hpp"
#include "Time.hpp"

namespace ftl
{

class SystemStream: public Stream
{
public:
	inline static Ref<SystemStream> create(int fd) {
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

	void write(StringList *parts, const char *sep = "");

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
