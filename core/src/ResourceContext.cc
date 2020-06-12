/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ResourceContext>
#include <cc/ThreadLocalSingleton>
#include <cc/Queue>

namespace cc {

ResourceContext *ResourceContext::instance()
{
    return ThreadLocalSingleton<ResourceContext>::instance();
}

ResourceContext::ResourceContext():
    queue_{Queue<String>::create()}
{}

void ResourceContext::push(const String &resource)
{
    queue_->pushBack(resource);
}

String ResourceContext::pop()
{
    if (queue_->count() == 0) return String{};
    return queue_->popBack();
}

String ResourceContext::top() const
{
    return queue_->count() > 0 ? queue_->back() : String{};
}

} // namespace cc
