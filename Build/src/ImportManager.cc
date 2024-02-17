/*
 * Copyright (C) 2024 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/build/ImportManager>
#include <cc/Map>

namespace cc::build {

struct ImportManager::State final: public Object::State
{
    State() = default;

    bool registerModule(const String &name, const String &source, Out<String> otherSource)
    {
        Locator pos;
        if (!modules_.insert(name, source, &pos)) {
            otherSource = modules_.at(pos).value();
            return false;
        }
        return true;
    }

    bool lookupModule(const String &name, Out<String> source)
    {
        return modules_.lookup(name, &source);
    }

    Map<String, String> modules_;
};

ImportManager::ImportManager()
{
    initOnce<State>();
}

bool ImportManager::lookupModule(const String &name, Out<String> source)
{
    return me().lookupModule(name, &source);
}

ImportManager::State &ImportManager::me()
{
    return Object::me.as<State>();
}

const ImportManager::State &ImportManager::me() const
{
    return Object::me.as<State>();
}

} // namespace cc::build
