/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Stream>

namespace cc {

template<class T> class Singleton;

/** \class NullStream NullStream.h cc/NullStream
  * \ingroup binary_io
  * \brief Zero storage information dump
  */
class NullStream: public Stream
{
public:
    static NullStream *instance();

    int read(ByteArray *data) override;
    void write(const ByteArray *data) override;

private:
    friend class Singleton<NullStream>;
    NullStream() {}
};

} // namespace cc
