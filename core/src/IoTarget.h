/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

namespace cc {

/** \class IoTarget IoTarget.h cc/IoTarget
  * \brief I/O event target
  */
class IoTarget
{
public:
    virtual int fd() const = 0;
protected:
    virtual ~IoTarget() {}
};

} // namespace cc
