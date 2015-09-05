/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_EXCEPTION_H
#define FLUX_EXCEPTION_H

#include <flux/String>

namespace flux {

/** \brief Root class of the exception class hierarchy
  * \see exceptions
  */
class Exception
{
public:
    ~Exception() throw() {}

    virtual String message() const = 0;
};

inline String str(const Exception &ex) { return ex.message(); }

} // namespace flux

#endif // FLUX_EXCEPTION_H
