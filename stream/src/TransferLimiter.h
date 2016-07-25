/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Stream>
#include <cc/Exception>

namespace cc {
namespace stream {

/** \brief Traffic limiting stream
  */
class TransferLimiter: public Stream
{
public:
    static Ref<TransferLimiter> open(Stream *stream, size_t readLimit, size_t writeLimit = 0);

    inline Stream *stream() const { return stream_; }
    inline size_t readLimit() const { return readLimit_; }
    inline size_t writeLimit() const { return writeLimit_; }
    inline size_t totalRead() const { return totalRead_; }
    inline size_t totalWritten() const { return totalWritten_; }

    virtual int read(ByteArray *data) override;
    virtual void write(const ByteArray *data) override;
    virtual void write(const StringList *parts) override;

private:
    TransferLimiter(Stream *stream, size_t readLimit, size_t writeLimit);

    Ref<Stream> stream_;
    size_t readLimit_;
    size_t writeLimit_;
    size_t totalRead_;
    size_t totalWritten_;
};

/** \brief General transfer limit exceeded exception
  */
class TransferLimitExceeded: public Exception
{
public:
    ~TransferLimitExceeded() throw() {}
};

/** \brief Exception thrown when a read limit is exceeded
  */
class ReadLimitExceeded: public TransferLimitExceeded
{
public:
    ~ReadLimitExceeded() throw() {}

    virtual String message() const { return "Read transfer limit exceeded"; }
};

/** \brief Exception thrown when a write limit is exceeded
  */
class WriteLimitExceeded: public TransferLimitExceeded
{
public:
    ~WriteLimitExceeded() throw() {}

    virtual String message() const { return "Write transfer limit exceeded"; }
};

}} // namespace cc::stream
