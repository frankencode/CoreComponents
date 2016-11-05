/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Object>
#include <cc/Ref>
#include <cc/SourceIterator>

namespace cc {

/** \brief Generic data source
  */
template<class T>
class Source: public Object
{
public:
    typedef T Item;
    virtual bool read(T *item) = 0;
};

} // namespace cc
