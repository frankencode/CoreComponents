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
#include "SyntaxState.h"
#include "ResourceContextStack.h"
#include "exceptions.h"

namespace flux
{

String DebugError::message() const
{
	return Format() << reason_ << " (" << String(source_)->fileName() << ":" << line_ << ")";
}

String systemError(int errorCode)
{
	String buf(1024);  // HACK, save bet
	const char *unknown = "Unknown error";
	memcpy(buf->chars(), unknown, strlen(unknown) + 1);
#ifdef __USE_GNU
	return strerror_r(errorCode, buf->chars(), buf->size());
#else
	/*int ret = */strerror_r(errorCode, buf->chars(), buf->size());
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
	return Format() << systemError(errorCode_) << " (" << String(source_)->fileName() << ":" << line_ << ")";
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

TextError::TextError(String text, int offset, String resource)
	: text_(text),
	  offset_(offset),
	  resource_(resource != "" ? resource : resourceContextStack()->top())
{}

SyntaxError::SyntaxError(String text, SyntaxState *state, String resource)
	: TextError(text, state ? state->hintOffset() : -1, resource),
	  state_(state)
{}

SyntaxError::~SyntaxError() throw()
{}

String SyntaxError::message() const
{
	Format format;
	const char *error = "Syntax error";
	if (state_) if (state_->hint()) {
		int line = 0, pos = 0;
		text_->offsetToLinePos(state_->hintOffset(), &line, &pos);
		if (resource_ != "") format << resource_ << ":";
		format << line << ":" << pos << ": ";
	}
	format << error;
	if (state_) if (state_->hint()) format << ": " << state_->hint();
	return format;
}

String SemanticError::message() const
{
	Format format;
	if (offset_ >= 0) {
		int line = 0, pos = 0;
		text_->offsetToLinePos(offset_, &line, &pos);
		if (resource_ != "") format << resource_ << ":";
		format << line << ":" << pos << ": ";
	}
	format << reason_;
	return format;
}

String Interrupt::message() const
{
	return Format() << "Caught signal " << signalName();
}

} // namespace flux
