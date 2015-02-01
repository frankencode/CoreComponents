/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXSTREAM_STREAMTAP_H
#define FLUXSTREAM_STREAMTAP_H

#include <flux/Stream>

namespace flux {
namespace stream {

/** \brief Bi-directional traffic redirection and duplication
  */
class StreamTap: public Stream
{
public:
    static Ref<StreamTap> open(Stream *stream, Stream *inputTap, Stream *outputTap);

    inline Stream *stream() const { return stream_; }

    virtual bool readyRead(double interval) const;
    virtual int read(ByteArray *buf);
    virtual void write(const ByteArray *buf);
    virtual void write(const StringList *parts);

private:
    StreamTap(Stream *stream, Stream *inputTap, Stream *outputTap);

    Ref<Stream> stream_, inputTap_, outputTap_;
};

}} // namespace flux::stream

#endif // FLUXSTREAM_STREAMTAP_H
