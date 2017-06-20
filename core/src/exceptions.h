/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <errno.h>
#include <cc/Exception>

namespace cc {

/** \class UsageError exceptions.h cc/exceptions
  * \brief %User input ambiguous, report back to user and provide guidance
  */
class UsageError: public Exception
{
public:
    UsageError(String message = String()): message_(message) {}
    ~UsageError() throw() {}

    virtual String message() const { return message_; }

private:
    String message_;
};

/** \class HelpError exceptions.h cc/exceptions
  * \brief %User requested help
  */
class HelpError: public Exception
{
public:
    ~HelpError() throw() {}

    virtual String message() const { return "No help, yet ..."; }
};

/** \class EncodingError exceptions.h cc/exceptions
  * \brief Some encoded data is mailformed
  */
class EncodingError: public Exception
{
public:
    ~EncodingError() throw() {}
};

/** \class UnexpectedEndOfInputError exceptions.h cc/exceptions
  * \brief End of input reached although more data is needed
  */
class UnexpectedEndOfInputError: public Exception
{
public:
    ~UnexpectedEndOfInputError() throw() {}

    virtual String message() const { return "Unexpected end of input"; }
};

/** \class BufferOverflow exceptions.h cc/exceptions
  * \brief Some buffer size is exceeded
  */
class BufferOverflow: public Exception
{
public:
    ~BufferOverflow() throw() {}

    virtual String message() const { return "Buffer overflow"; }
};

/** \class DebugError exceptions.h cc/exceptions
  * \brief Debugging hint on internal system malfunction
  */
class DebugError: public Exception
{
public:
    DebugError(String reason, const char *source, int line):
        reason_(reason),
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

String systemError(int errorCode);

/** \class SystemError exceptions.h cc/exceptions
  * \brief %System call failed
  */
class SystemError: public Exception
{
public:
    SystemError(int errorCode): errorCode_(errorCode) {}
    ~SystemError() throw() {}

    inline int errorCode() const { return errorCode_; }

    virtual String message() const;

protected:
    int errorCode_;
};

/** \class SystemResourceError exceptions.h cc/exceptions
  * \brief %System call failed to perform an action on a named resource (e.g. a file)
  */
class SystemResourceError: public SystemError
{
public:
    SystemResourceError(int errorCode, String resource, const char *source, int line):
        SystemError(errorCode),
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

/** \class SystemDebugError exceptions.h cc/exceptions
  * \brief Debugging hint on system call failure
  */
class SystemDebugError: public SystemError
{
public:
    SystemDebugError(int errorCode, const char *source, int line):
        SystemError(errorCode),
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

#define CC_DEBUG_ERROR(reason) \
    throw DebugError(reason, __FILE__, __LINE__)

#define CC_SYSTEM_RESOURCE_ERROR(errorCode, resource) \
    throw SystemResourceError(errorCode, resource, __FILE__, __LINE__)

#define CC_SYSTEM_DEBUG_ERROR(errorCode) \
    throw SystemDebugError(errorCode, __FILE__, __LINE__)

#define CC_SYSTEM_ERROR(errorCode, resource) \
    { \
        if (resource != "") CC_SYSTEM_RESOURCE_ERROR(errorCode, resource); \
        else CC_SYSTEM_DEBUG_ERROR(errorCode); \
    }

/** \class TextError exceptions.h cc/exceptions
  * \brief General error related to a text (progam text, config file, etc.)
  */
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

/** \class SemanticError exceptions.h cc/exceptions
  * \brief Semantic error
  */
class SemanticError: public TextError
{
public:
    SemanticError(String reason, String text = "", int offset = -1, String resource = ""):
        TextError(text, offset, resource),
        reason_(reason)
    {}
    ~SemanticError() throw() {}

    inline String reason() const { return reason_; }

    virtual String message() const;

private:
    String reason_;
};

String signalName(int signal);

/** \class Interrupt exceptions.h cc/exceptions
  * \brief Signal received
  */
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

/** \class Timeout exceptions.h cc/exceptions
  * \brief Operation timed out
  */
class Timeout: public Exception
{
public:
    ~Timeout() throw() {}
    virtual String message() const;
};

/** \class ConnectionResetByPeer exceptions.h cc/exceptions
  * \brief Other end of a communication channel was closed (pipe or socket)
  */
class ConnectionResetByPeer: public Exception
{
public:
    ~ConnectionResetByPeer() throw() {}
    virtual String message() const;
};

/** \class TransferError exceptions.h cc/exceptions
  * \brief A data transfer operation failed
  * \see Transfer
  */
class TransferError: public Exception
{
public:
    TransferError(String details);
    ~TransferError() throw() {}

    virtual String message() const;

private:
    String details_;
};

/** \class PermissionError exceptions.h cc/exceptions
  * \brief Insufficient permission to perform operation
  */
class PermissionError: public UsageError
{
public:
    ~PermissionError() throw() {}
    virtual String message() const;
};

/** \class CommandNotFound exceptions.h cc/exceptions
  * \brief Command not found error
  */
class CommandNotFound: public UsageError
{
public:
    CommandNotFound(String command = ""):
        command_(command)
    {}
    ~CommandNotFound() throw() {}

    String command() const { return command_; }

    virtual String message() const;

private:
    String command_;
};

} // namespace cc
