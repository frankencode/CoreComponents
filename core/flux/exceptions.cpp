/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <string.h> // strerror_r
#include "Format.h"
#include "Thread.h"
#include "exceptions.h"

namespace flux
{

String DebugError::message() const
{
	return Format() << String(source_)->fileName() << ":" << line_ << ": " << reason_;
}

char *systemError(int errorCode)
{
#ifdef __USE_GNU
	const char *unknown = "Unknown error";
	const int bufSize = 1024; // HACK, save bet
	char buf[bufSize];
	memcpy(buf, unknown, strlen(unknown) + 1);
	return strdup(strerror_r(errorCode, buf, bufSize));
#else
	const char *unknown = "Unknown error";
	const int bufSize = 1024;
	char *buf = new char[bufSize];
	memcpy(buf, unknown, strlen(unknown) + 1);
	/*int ret = */strerror_r(errorCode, buf, bufSize);
	return buf;
#endif
}

String SystemResourceError::message() const
{
	return Format() << systemError(errorCode_) << ": \"" << resource_ << "\""
		#ifndef NDEBUG
		<< " (" << String(source_)->fileName() << ":" << line_ << ")"
		#endif
		;
}

String SystemDebugError::message() const
{
	return Format() << String(source_)->fileName() << ":" << line_ << ": " << systemError(errorCode_);
}

Interrupt::Interrupt()
{
	__sync_synchronize();
	signal_ = Thread::self()->lastSignal_;
}

const char *Interrupt::signalName() const
{
	switch (signal_) {
		case SIGHUP:  return "SIGHUP";
		case SIGINT:  return "SIGINT";
		case SIGQUIT: return "SIGQUIT";
		case SIGILL:  return "SIGILL";
		case SIGABRT: return "SIGABRT";
		case SIGFPE:  return "SIGFPE";
		case SIGKILL: return "SIGKILL";
		case SIGSEGV: return "SIGSEGV";
		case SIGPIPE: return "SIGPIPE";
		case SIGALRM: return "SIGALRM";
		case SIGTERM: return "SIGTERM";
		case SIGUSR1: return "SIGUSR1";
		case SIGUSR2: return "SIGUSR2";
		case SIGCHLD: return "SIGCHLD";
		case SIGCONT: return "SIGCONT";
		case SIGSTOP: return "SIGSTOP";
		case SIGTSTP: return "SIGTSTP";
		case SIGTTIN: return "SIGTTIN";
		case SIGTTOU: return "SIGTTOU";
		case SIGBUS:  return "SIGBUS";
	}
	return "SIG???";
}

String Interrupt::message() const
{
	return Format() << "Caught signal " << signalName();
}

String Timeout::message() const
{
	return "Operation timed out";
}

String ProcessError::message() const
{
	return Format() << "Process unsuccessful: %% (status = %%)" << command_ << status_;
}

} // namespace flux
