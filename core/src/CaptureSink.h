/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Stream>

namespace cc {

/** \class CaptureSink CaptureSink.h cc/CaptureSink
  * \ingroup binary_io
  * \brief Capture output in memory
  * \see ReplaySource, ByteSink
  */
class CaptureSink: public Stream
{
public:
    /** Open a new CaptureSink
      * \return new object instance
      */
    static Ref<CaptureSink> open() { return new CaptureSink; }

    void write(const ByteArray *data) override;

    /** Collect all data written to this sink
      * \return Captured output
      */
    Ref<ByteArray> collect() const;

private:
    CaptureSink();

    Ref<StringList> parts_;
};

} // namespace cc
