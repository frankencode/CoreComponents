/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/ThreadLocalSingleton>
#include <flux/Queue>
#include <flux/ResourceContext>

namespace flux {

ResourceContext::ResourceContext()
    : queue_(Queue<String>::create())
{}

void ResourceContext::push(String resource)
{
    queue_->pushBack(resource);
}

String ResourceContext::pop()
{
    if (queue_->count() == 0) return String();
    return queue_->popBack();
}

String ResourceContext::top() const
{
    return queue_->count() > 0 ? queue_->back() : String();
}

ResourceContext *resourceContext()
{
    return ThreadLocalSingleton<ResourceContext>::instance();
}

} // namespace flux
