/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/ResourceContext>
#include <cc/Queue>

namespace cc {

struct ResourceContext::State: public Object::State
{
    Queue<String> queue_;
};

ResourceContext::ResourceContext()
{
    initOncePerThread<State>();
}

void ResourceContext::push(const String &resource)
{
    me().queue_.pushBack(resource);
}

String ResourceContext::pop()
{
    if (me().queue_.count() == 0) return String{};
    String resource = me().queue_.last();
    me().queue_.popBack();
    return resource;
}

String ResourceContext::top() const
{
    return me().queue_.count() > 0 ? me().queue_.last() : String{};
}

ResourceContext::State &ResourceContext::me()
{
    return Object::me.as<State>();
}

const ResourceContext::State &ResourceContext::me() const
{
    return Object::me.as<State>();
}

} // namespace cc
