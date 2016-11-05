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
    static Ref<LineSource> open(ByteArray *buf);
    static Ref<LineSource> open(Stream *stream, ByteArray *buf = 0);

    inline Stream *stream() const { return stream_; }
    inline ByteArray *buf() const { return buf_; }

    bool read(String *line);
    String readLine();

    String pendingData() const;

private:
    LineSource(Stream *stream, ByteArray *buf);

    int findEol(ByteArray *buf, int n, int i) const;
    int skipEol(ByteArray *buf, int n, int i) const;

    Ref<Stream> stream_;
    String buf_;
    bool eoi_;
    int i_, n_;
};

CC_DEFINE_SOURCE_BEGIN_END(LineSource);

} // namespace cc
