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

/** \class HashSink HashSink.h cc/HashSink
  * \brief Hashsum computing sink
  */
class HashSink: public Stream
{
public:
    virtual Ref<ByteArray> finish() = 0;
};

} // namespace cc
