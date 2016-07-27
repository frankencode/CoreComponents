/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/String>

namespace cc {

/** \class Exception Exception.h cc/Exception
  * \brief Root class of the exception class hierarchy
  * \see exceptions
  */
class Exception
{
public:
    ~Exception() throw() {}

    virtual String message() const = 0;
};

inline String str(const Exception &ex) { return ex.message(); }

} // namespace cc
