/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_EXCEPTIONS_H
#define FLUX_EXCEPTIONS_H

#include <errno.h>
#include <flux/Exception>

namespace flux {

class UsageError: public Exception
{
public:
    UsageError(String message): message_(message) {}
    ~UsageError() throw() {}

    virtual String message() const { return message_; }

private:
    String message_;
};

class HelpError: public Exception
{
public:
    ~HelpError() throw() {}

    virtual String message() const { return "No help, yet ..."; }
};

class EncodingError: public Exception
{
public:
    ~EncodingError() throw() {}
};

class UnexpectedEndOfInputError: public Exception
{
public:
    ~UnexpectedEndOfInputError() throw() {}

    virtual String message() const { return "Unexpected end of input"; }
};

class BufferOverflow: public Exception
{
public:
    ~BufferOverflow() throw() {}

    virtual String message() const { return "Buffer overflow"; }
};

class DebugError: public Exception
{
public:
    DebugError(String reason, const char *source, int line)
        : reason_(reason),
          source_(source),
          line_(line)
    {}
    ~DebugError() throw() {}

    virtual String message() const;

private:
    String reason_;
    const char *source_;
    int line_;
};

class SystemError: public Exception
{
public:
    SystemError(int errorCode): errorCode_(errorCode) {}
    ~SystemError() throw() {}

    inline int errorCode() const { return errorCode_; }

protected:
    int errorCode_;
};

class SystemResourceError: public SystemError
{
public:
    SystemResourceError(int errorCode, String resource, const char *source, int line)
        : SystemError(errorCode),
          resource_(resource),
          source_(source),
          line_(line)
    {}
    ~SystemResourceError() throw() {}

    inline String resource() const { return resource_; }
    inline const char *source() const { return source_; }
    inline int line() const { return line_; }

    virtual String message() const;

private:
    String resource_;
    const char *source_;
    int line_;
};

class SystemDebugError: public SystemError
{
public:
    SystemDebugError(int errorCode, const char *source, int line)
        : SystemError(errorCode),
          source_(source),
          line_(line)
    {}
    ~SystemDebugError() throw() {}

    inline const char *source() const { return source_; }
    inline int line() const { return line_; }

    virtual String message() const;

private:
    const char *source_;
    int line_;
};

#define FLUX_DEBUG_ERROR(reason) \
    throw DebugError(reason, __FILE__, __LINE__)

#define FLUX_SYSTEM_RESOURCE_ERROR(errorCode, resource) \
    throw SystemResourceError(errorCode, resource, __FILE__, __LINE__)

#define FLUX_SYSTEM_DEBUG_ERROR(errorCode) \
    throw SystemDebugError(errorCode, __FILE__, __LINE__)

#define FLUX_SYSTEM_ERROR(errorCode, resource) \
    { \
        if (resource != "") FLUX_SYSTEM_RESOURCE_ERROR(errorCode, resource); \
        else FLUX_SYSTEM_DEBUG_ERROR(errorCode); \
    }

class TextError: public Exception
{
public:
    inline String text() const { return text_; }
    inline int offset() const { return offset_; }
    inline String resource() const { return resource_; }

    void setResource(String resource) { resource_ = resource; }

protected:
    TextError(String text, int offset, String resource = "");
    ~TextError() throw() {}

    String text_;
    int offset_;
    String resource_;
};

class SyntaxState;

class SyntaxError: public TextError
{
public:
    SyntaxError(String text, SyntaxState *state = 0, String resource = "");
    ~SyntaxError() throw();

    inline SyntaxState *state() const { return state_; }

    virtual String message() const;

private:
    Ref<SyntaxState> state_;
};

class SemanticError: public TextError
{
public:
    SemanticError(String reason, String text = "", int offset = -1, String resource = "")
        : TextError(text, offset, resource),
          reason_(reason)
    {}
    ~SemanticError() throw() {}

    inline String reason() const { return reason_; }

    virtual String message() const;

private:
    String reason_;
};

String signalName(int signal);

class Interrupt: public Exception
{
public:
    Interrupt(int signal);
    ~Interrupt() throw() {}

    inline int signal() const { return signal_; }
    String signalName() const;

    virtual String message() const;

private:
    int signal_;
};

class Timeout: public Exception
{
public:
    ~Timeout() throw() {}

    virtual String message() const { return "Operation timed out"; }
};

} // namespace flux

#endif // FLUX_EXCEPTIONS_H
