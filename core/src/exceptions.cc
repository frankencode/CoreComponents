/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <string.h> // strerror_r
#include <signal.h>
#include <cc/Format>
#include <cc/ResourceContext>
#include <cc/exceptions>

namespace cc {

String DebugError::message() const
{
    return Format{} << reason_ << " (" << String{source_}->fileName() << ":" << line_ << ")";
}

String systemError(int errorCode)
{
    String buf{1023};  // HACK, save bet
    const char *unknown = "Unknown error";
    memcpy(mutate(buf)->chars(), unknown, strlen(unknown) + 1);
#ifdef __USE_GNU
    return strerror_r(errorCode, mutate(buf)->chars(), buf->count());
#else
    /*int ret = */strerror_r(errorCode, mutate(buf)->chars(), buf->count());
    return buf;
#endif
}

String SystemError::message() const
{
    return systemError(errorCode_);
}

String SystemResourceError::message() const
{
    return Format{} << systemError(errorCode_) << ": \"" << resource_ << "\""
        #ifndef NDEBUG
        << " (" << String(source_)->fileName() << ":" << line_ << ")"
        #endif
        ;
}

String SystemDebugError::message() const
{
    return Format() << systemError(errorCode_) << " (" << String(source_)->fileName() << ":" << line_ << ")";
}

TextError::TextError(const String &text, int offset, const String &resource):
    text_{text},
    offset_{offset},
    resource_{resource != "" ? resource : ResourceContext::instance()->top()}
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

String Timeout::message() const
{
    return "Operation timed out";
}

String ConnectionResetByPeer::message() const
{
    return "Connection reset by peer";
}

TransferError::TransferError(const String &details):
    details_{details}
{}

String TransferError::message() const
{
    return String{"Data transfer failed: "} + details_;
}

String PermissionError::message() const
{
    return "Insufficient permission to perform operation";
}

String CommandNotFound::message() const
{
    return Format{"Command not found: '%%'"} << command_;
}

} // namespace cc
