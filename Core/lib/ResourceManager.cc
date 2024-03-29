/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/ResourceManager>
#include <cc/Map>
#include <cc/Prefix>

namespace cc {

struct ResourceManager::State final: public Singleton::State
{
    void addDirectory(const String &realPrefix, const String &resourcePrefix)
    {
        pathMap_.establish(resourcePrefix != "" ? resourcePrefix : realPrefix.fileName(), realPrefix);
    }

    bool lookup(const String &resourcePath, Out<String> realPath) const
    {
        if (!resourcePath.startsWith(":/")) return resourcePath;

        Locator target;
        bool found = pathMap_.find(Prefix{resourcePath, 2}, &target);

        if (found) {
            String resourcePrefix = pathMap_.at(target).key();
            String realPrefix = pathMap_.at(target).value();
            String realSuffix = resourcePath.copy(2 + resourcePrefix.count(), resourcePath.count());
            realPath = realPrefix + realSuffix;
        }

        return found;
    }

    Map<String, String> pathMap_;
};

ResourceManager::ResourceManager():
    Singleton{instance<State>()}
{}

ResourceManager &ResourceManager::addDirectory(const String &realPrefix, const String &resourcePrefix)
{
    me().addDirectory(realPrefix, resourcePrefix);
    return *this;
}

bool ResourceManager::lookup(const String &resourcePath, Out<String> realPath) const
{
    return me().lookup(resourcePath, &realPath);
}

String ResourceManager::realPath(const String &resourcePath) const
{
    String realPath;
    lookup(resourcePath, &realPath);
    return realPath;
}

ResourceManager::State &ResourceManager::me()
{
    return Object::me.as<State>();
}

const ResourceManager::State &ResourceManager::me() const
{
    return Object::me.as<State>();
}

} // namespace cc
