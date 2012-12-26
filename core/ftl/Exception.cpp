/*
 * Exception.cpp -- exception generators
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */

#include <errno.h>
#include <string.h> // strerror_r
#include "types.hpp"
#include "strings.hpp"
#include "Thread.hpp"
#include "Exception.hpp"

namespace ftl
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

const char *Exception::what() const throw() {
	if (!message_) {
		const char *path = fileName(path_);
		char *lineStr = intToStr(line_);
		message_ = str::cat(path, ":", lineStr, ": ", className_, ": ", reason_);
		delete[] lineStr;
		if ((reason_) && reasonOnHeap_) {
			delete[] reason_;
			reason_ = 0;
		}
	}
	return message_;
}

char *captureExceptionMessage(const char *s) { return str::dup(s); }
char *captureExceptionMessage(char *s) { return s; }

char *systemError() { return systemError(errno); }

char *systemError(int errorCode)
{
#ifdef __USE_GNU
	const char *unknown = "Unknown error";
	const int bufSize = 1024; // HACK, save bet
	char buf[bufSize];
	mem::cpy(buf, unknown, str::len(unknown) + 1);
	return str::dup(::strerror_r(errorCode, buf, bufSize));
#else
	const char *unknown = "Unknown error";
	const int bufSize = 1024;
	char *buf = new char[bufSize];
	mem::cpy(buf, unknown, str::len(unknown) + 1);
	/*int ret = */::strerror_r(errorCode, buf, bufSize);
	return buf;
#endif
}

char *pthreadError(const char *callName, int errorCode)
{
	char *errorStr = systemError(errorCode);
	char *errorCodeStr = ftl::intToStr(errorCode);
	char *msg = str::cat(callName, "() failed: ", errorStr, " (", errorCodeStr, ")");
	delete[] errorStr;
	delete[] errorCodeStr;
	return msg;
}

Interrupt::Interrupt()
{
	__sync_synchronize();
	signal_ = Thread::self()->lastSignal_;
}

} // namespace ftl
