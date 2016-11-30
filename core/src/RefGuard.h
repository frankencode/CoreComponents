/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Ref>

namespace cc {

/** \class RefGuard RefGuard.h cc/RefGuard
  * \ingroup mman
  * \brief %Guard against object destruction
  */
template<class T>
class RefGuard
{
public:
    RefGuard(Ref<T> *ref): ref_(ref) {}
    ~RefGuard() { *ref_ = 0; }
private:
    Ref<T> *ref_;
};

} // namespace cc
