/*
 * Copyright (C) 2024 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/build/SystemImportManager>
#include <cc/DirWalk>
#include <cc/Map>

namespace cc::build {

struct SystemImportManager::State final: public Object::State
{
    State()
    {
        globModules("/usr/local/import");
        globModules("/usr/import");
    }

    void globModules(const String &importPrefix)
    {
        for (const String &path: DirWalk{importPrefix, DirWalk::FilesOnly}) {
            String name = path.copy(importPrefix.count() + 1, path.count());
            name.replace('/', '.');
            modules_.insert(name, path);
        }
    }

    bool lookupModule(const String &module, Out<String> source) const
    {
        return modules_.lookup(module, &source);
    }

    Map<String, String> modules_;
};

SystemImportManager::SystemImportManager()
{
    initOnce<State>();
}

bool SystemImportManager::lookupModule(const String &module, Out<String> source) const
{
    return me().lookupModule(module, &source);
}

const SystemImportManager::State &SystemImportManager::me() const
{
    return Object::me.as<State>();
}

} // namespace cc::build
