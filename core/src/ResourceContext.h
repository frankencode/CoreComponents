/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_RESOURCECONTEXT_H
#define FLUX_RESOURCECONTEXT_H

#include <flux/String>

namespace flux {

template<class> class ThreadLocalSingleton;
template<class> class Queue;

/** \brief Cascaded resource context for error reporting
  * \see ResourceGuard
  */
class ResourceContext: public Object
{
public:
    void push(String resource);
    String pop();

    String top() const;

private:
    friend class ThreadLocalSingleton<ResourceContext>;
    ResourceContext();

    Ref< Queue<String> > queue_;
};

ResourceContext *resourceContextStack();

} // namespace flux

#endif // FLUX_RESOURCECONTEXT_H
