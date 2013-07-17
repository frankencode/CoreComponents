/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <errno.h>
#include <string.h> // strerror_r
#include "types.h"
#include "strings.h"
#include "Thread.h"
#include "Exception.h"

namespace fkit
{

const char *fileName(const char *path)
{
	char sep = '/';

	const char *p = path;
	const char *pSaved = path;
	while (*p)
	{
		if (*p == sep)
			pSaved = p + 1;
		++p;
	}

	return pSaved;
}

Exception::Exception()
	: message_(0)
{}

Exception::Exception(const char *path, int line, const char *className, const char *reason, bool reasonOnHeap)
	: path_(path),
	  line_(line),
	  className_(className),
	  reason_(reason),
	  reasonOnHeap_(reasonOnHeap),
	  message_(0)
{}

Exception::~Exception() throw()
{
	if ((reason_) && reasonOnHeap_)
		delete[] reason_;
	if (message_)
		delete[] message_;
}

const char *Exception::what() const throw()
{
	return message();
}

const char *Exception::message() const throw()
{
	if (!message_) {
		const char *path = fileName(path_);
		char *lineStr = intToStr(line_);
		message_ = strcat(path, ":", lineStr, ": ", className_, ": ", reason_);
		delete[] lineStr;
		if ((reason_) && reasonOnHeap_) {
			delete[] reason_;
			reason_ = 0;
		}
	}
	return message_;
}

char *captureExceptionMessage(const char *s) { return strdup(s); }
char *captureExceptionMessage(char *s) { return s; }

char *systemError() { return systemError(errno); }

char *systemError(int errorCode)
{
#ifdef __USE_GNU
	const char *unknown = "Unknown error";
	const int bufSize = 1024; // HACK, save bet
	char buf[bufSize];
	memcpy(buf, unknown, strlen(unknown) + 1);
	return strdup(::strerror_r(errorCode, buf, bufSize));
#else
	const char *unknown = "Unknown error";
	const int bufSize = 1024;
	char *buf = new char[bufSize];
	memcpy(buf, unknown, strlen(unknown) + 1);
	/*int ret = */::strerror_r(errorCode, buf, bufSize);
	return buf;
#endif
}

char *pthreadError(const char *callName, int errorCode)
{
	char *errorStr = systemError(errorCode);
	char *errorCodeStr = fkit::intToStr(errorCode);
	char *msg = strcat(callName, "() failed: ", errorStr, " (", errorCodeStr, ")");
	delete[] errorStr;
	delete[] errorCodeStr;
	return msg;
}

Interrupt::Interrupt()
{
	__sync_synchronize();
	signal_ = Thread::self()->lastSignal_;
}

} // namespace fkit
