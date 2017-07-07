/*
 * Copyright (C) 2007-2017 Frank Mertens.
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

/** \class Source Source.h cc/Source
  * \brief Generic data source
  * \tparam T item type
  */
template<class T>
class Source: public Object
{
public:
    /// Item type
    typedef T Item;

    /** Read an item from the source
      * \param item return next item if not end of input
      * \return true if not end of input, false otherwise
      */
    virtual bool read(T *item) = 0;
};

} // namespace cc
