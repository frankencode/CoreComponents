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

/** \class HashSum HashSum.h cc/HashSum
  * \brief Hash sum interface
  */
class HashSum: public Stream
{
public:
    virtual Ref<ByteArray> finish() = 0;
};

} // namespace cc
