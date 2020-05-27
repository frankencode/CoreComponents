/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <string.h> // strerror_r
#include <signal.h>
#include <cc/format>
#include <cc/ResourceContext>
#include <cc/exceptions>

namespace cc {

string DebugError::message() const
{
    return format{} << reason_ << " (" << string{source_}->fileName() << ":" << line_ << ")";
}

string systemError(int errorCode)
{
    string buf{1023};  // HACK, save bet
    const char *unknown = "Unknown error";
    memcpy(mutate(buf)->chars(), unknown, strlen(unknown) + 1);
#ifdef __USE_GNU
    return strerror_r(errorCode, mutate(buf)->chars(), buf->count());
#else
    /*int ret = */strerror_r(errorCode, mutate(buf)->chars(), buf->count());
    return buf;
#endif
}

string SystemError::message() const
{
    return systemError(errorCode_);
}

string SystemResourceError::message() const
{
    return format{} << systemError(errorCode_) << ": \"" << resource_ << "\""
        #ifndef NDEBUG
        << " (" << string{source_}->fileName() << ":" << line_ << ")"
        #endif
        ;
}

string SystemDebugError::message() const
{
    return format() << systemError(errorCode_) << " (" << string{source_}->fileName() << ":" << line_ << ")";
}

TextError::TextError(const string &text, int offset, const string &resource):
    text_{text},
    offset_{offset},
    resource_{resource != "" ? resource : ResourceContext::instance()->top()}
{}

string SemanticError::message() const
{
    format format;
    if (offset_ >= 0) {
        int line = 0, pos = 0;
        text_->offsetToLinePos(offset_, &line, &pos);
        if (resource_ != "") format << resource_ << ":";
        format << line << ":" << pos << ": ";
    }
    format << reason_;
    return format;
}

string Timeout::message() const
{
    return "Operation timed out";
}

string ConnectionResetByPeer::message() const
{
    return "Connection reset by peer";
}

TransferError::TransferError(const string &details):
    details_{details}
{}

string TransferError::message() const
{
    return string{"Data transfer failed: "} + details_;
}

string PermissionError::message() const
{
    return "Insufficient permission to perform operation";
}

string CommandNotFound::message() const
{
    return format{"Command not found: '%%'"} << command_;
}

} // namespace cc
