/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <string.h> // strerror_r
#include <signal.h>
#include <flux/Format>
#include <flux/ResourceContext>
#include <flux/exceptions>

namespace flux {

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
    return strerror_r(errorCode, buf->chars(), buf->count());
#else
    /*int ret = */strerror_r(errorCode, buf->chars(), buf->count());
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

TextError::TextError(String text, int offset, String resource)
    : text_(text),
      offset_(offset),
      resource_(resource != "" ? resource : resourceContext()->top())
{}

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

String signalName(int signal)
{
    switch (signal) {
        case SIGHUP:   return "SIGHUP";
        case SIGINT:   return "SIGINT";
        case SIGQUIT:  return "SIGQUIT";
        case SIGILL:   return "SIGILL";
        case SIGABRT:  return "SIGABRT";
        case SIGFPE:   return "SIGFPE";
        case SIGKILL:  return "SIGKILL";
        case SIGSEGV:  return "SIGSEGV";
        case SIGPIPE:  return "SIGPIPE";
        case SIGALRM:  return "SIGALRM";
        case SIGTERM:  return "SIGTERM";
        case SIGUSR1:  return "SIGUSR1";
        case SIGUSR2:  return "SIGUSR2";
        case SIGCHLD:  return "SIGCHLD";
        case SIGCONT:  return "SIGCONT";
        case SIGSTOP:  return "SIGSTOP";
        case SIGTSTP:  return "SIGTSTP";
        case SIGTTIN:  return "SIGTTIN";
        case SIGTTOU:  return "SIGTTOU";
        case SIGBUS:   return "SIGBUS";
        case SIGWINCH: return "SIGWINCH";
    }
    return str(signal);
}


Interrupt::Interrupt(int signal)
    : signal_(signal)
{}

String Interrupt::signalName() const
{
    return flux::signalName(signal_);
}

String Interrupt::message() const
{
    return Format() << "Caught signal " << signalName();
}

} // namespace flux
