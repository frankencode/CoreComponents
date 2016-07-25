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

template<class T> class Singleton;

/** \brief Zero storage information dump
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
