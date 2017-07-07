/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/String>

namespace cc {

template<class> class ThreadLocalSingleton;
template<class> class Queue;

/** \class ResourceContext ResourceContext.h cc/ResourceContext
  * \brief Cascaded resource context for error reporting
  * \see ResourceGuard
  */
class ResourceContext: public Object
{
public:
    static ResourceContext *instance();

    void push(String resource);
    String pop();

    String top() const;

private:
    friend class ThreadLocalSingleton<ResourceContext>;
    ResourceContext();

    Ref< Queue<String> > queue_;
};

} // namespace cc
