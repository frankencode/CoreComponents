/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Source>
#include <cc/Stream>

namespace cc {

/** \class LineSource LineSource.h cc/LineSource
  * \brief Line input buffer
  */
class LineSource: public Source<String>
{
public:
    /** Open a new line source feed from a memory buffer
      * \param buffer input buffer
      * \return new object instance
      */
    static Ref<LineSource> open(ByteArray *buffer);

    /** Open a new line source feed from an input stream
      * \param stream input stream
      * \param buffer input buffer (optional)
      * \return new object instance
      */
    static Ref<LineSource> open(Stream *stream, ByteArray *buffer = 0);

    /// Input stream used to read lines from
    inline Stream *stream() const { return stream_; }

    /// Input buffer used to cache input data
    inline ByteArray *buffer() const { return buffer_; }

    /** Read next line
      * \param line returns the next line (if not end of input)
      * \return true if not end of input, false otherwise
      */
    bool read(String *line);

    /** Read next line
      * \return next line (equals "" on end of input)
      */
    String readLine();

    /// Return the remaining data from the input buffer
    String pendingData() const;

private:
    LineSource(Stream *stream, ByteArray *buffer);

    int findEol(ByteArray *buffer, int n, int i) const;
    int skipEol(ByteArray *buffer, int n, int i) const;

    Ref<Stream> stream_;
    Ref<ByteArray> buffer_;
    bool eoi_;
    int i_, n_;
};

CC_DEFINE_SOURCE_BEGIN_END(LineSource);

} // namespace cc
