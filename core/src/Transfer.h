/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/exceptions>
#include <cc/Stream>

namespace cc {

class TransferInputWorker;
class TransferOutputWorker;

/** \class Transfer Transfer.h cc/Transfer
  * \brief Non-trivial data transfer
  * \see Stream
  * To achieve high performance data transfers reading from a source and writing to a sink has to be decoupled
  * into concurrent activities. Thereby waiting for pending writes to complete and waiting for new data to become
  * available from the source can happen at the same time.
  */
class Transfer: public Object
{
public:
    /** Start a new data transfer
      * \param source data source to read from
      * \param sink data sink to write to
      * \param bufferSize individual I/O buffer size
      * \param bufferCount number of I/O buffers to use
      * \return new object instance
      */
    static Ref<Transfer> start(Stream *source, Stream *sink, int bufferSize = 0x8000, int bufferCount = 10);

    /** Stop the data transfer
      */
    void stop();

    /** Wait for completion of the data transfer
      * \exception TransferError
      */
    void waitComplete();

private:
    Transfer(Stream *source, Stream *sink, int bufferSize, int bufferCount);

    Ref<TransferInputWorker> inputWorker_;
    Ref<TransferOutputWorker> outputWorker_;
};

} // namespace cc
