/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FKIT_SYSTEMSTREAM_H
#define FKIT_SYSTEMSTREAM_H

#include "Stream.h"

namespace fkit
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

	bool readyRead(double timeout);
	bool readyReadOrWrite(double timeout);

	int readAvail(ByteArray *buf);
	void write(const ByteArray *buf);

	void write(StringList *parts, const char *sep = "");

	inline void write(String s) { write(s.get()); }

	void closeOnExec();

protected:
	SystemStream(int fd);

	int fd_;
	mutable bool isattyCached_, isatty_;
};

} // namespace fkit

#endif // FKIT_SYSTEMSTREAM_H
