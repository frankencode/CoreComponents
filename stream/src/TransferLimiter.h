/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXSTREAM_TRANSFERLIMITER_H
#define FLUXSTREAM_TRANSFERLIMITER_H

#include <flux/Stream>
#include <flux/Exception>

namespace flux {
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

    virtual bool readyRead(double interval) const;
    virtual int read(ByteArray *buf);
    virtual void write(const ByteArray *buf);
    virtual void write(const StringList *parts);

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

}} // namespace flux::stream

#endif // FLUXSTREAM_TRANSFERLIMITER_H
