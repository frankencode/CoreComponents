/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ThreadLocalSingleton>
#include <cc/Queue>
#include <cc/ResourceContext>

namespace cc {

ResourceContext *ResourceContext::instance()
{
    return ThreadLocalSingleton<ResourceContext>::instance();
}

ResourceContext::ResourceContext():
    queue_{Queue<string>::create()}
{}

void ResourceContext::push(const string &resource)
{
    queue_->pushBack(resource);
}

string ResourceContext::pop()
{
    if (queue_->count() == 0) return string{};
    return queue_->popBack();
}

string ResourceContext::top() const
{
    return queue_->count() > 0 ? queue_->back() : string{};
}

} // namespace cc
