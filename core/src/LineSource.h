/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_LINESOURCE_H
#define FLUX_LINESOURCE_H

#include <flux/String>
#include <flux/Stream>

namespace flux {

/** \brief Line input buffer
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

} // namespace flux

#endif // FLUX_LINESOURCE_H
