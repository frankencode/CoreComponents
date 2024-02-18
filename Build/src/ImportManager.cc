/*
 * Copyright (C) 2024 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/build/ImportManager>
#include <cc/build/BuildPlan>
#include <cc/build/ToolChain>
#include <cc/build/JobScheduler>
#include <cc/FileInfo>
#include <cc/DirWalk>
#include <cc/Dir>
#include <cc/Map>
#include <cc/Mutex>
#include <cc/Guard>
#include <cc/DEBUG>

namespace cc::build {

struct ImportManager::State final: public Object::State
{
    State()
    {
        gatherModules("/usr/local/import");
        gatherModules("/usr/import");

        Dir::establish(cachePrefix_);
    }

    void gatherModules(const String &importPrefix)
    {
        for (const String &source: DirWalk{importPrefix, DirWalk::FilesOnly}) {
            registerModule(moduleName(importPrefix, source), source);
        }
    }

    bool registerModule(const String &name, const String &source, Out<String> otherSource = None{})
    {
        Guard guard { mutex_ };

        Locator pos;
        if (!modules_.insert(name, source, &pos)) {
            otherSource = modules_.at(pos).value().source_;
            return false;
        }
        return true;
    }

    void registerInclude(const String &name, const String &include)
    {
        Guard guard { mutex_ };

        Module module;
        if (modules_.lookup(name, &module)) {
            module.includes_.append(include);
        }
    }

    bool lookupModule(const String &name, Out<String> source)
    {
        Guard guard { mutex_ };

        Module module;
        bool found = modules_.lookup(name, &module);
        if (found) source = module.source_;
        return found;
    }

    static String cachePath(const String &module)
    {
        String path = module;
        if (path.contains('/')) {
            path.replace("./", ",/");
            path.replace("../", ",,/");
            // if (path.startsWith('/')) path = "." + path;
            while (path.startsWith('/')) path = path.copy(1, path.count());
        }
        path += ".gcm";
        return path;
    }

    bool compileHeaderUnit(JobScheduler &scheduler, const BuildPlan &plan, const String &source, Out<String> cachePath) const
    {
        FileInfo srcInfo { source };
        if (!srcInfo) {
            return false;
        }
        cachePath = State::cachePath(source);
        FileInfo dstInfo { cachePrefix_ / *cachePath };
        if (dstInfo && srcInfo.lastModified() <= dstInfo.lastModified()) {
            return true;
        }
        Job job { plan.toolChain().headerUnitCompileCommand(plan, source) };
        bool ok = job.run();
        scheduler.report(job);
        return ok;
    }

    bool compileInterfaceUnit(JobScheduler &scheduler, const BuildPlan &plan, const String &name, Out<String> cachePath) const
    {
        Guard guard { mutex_ };

        Module module;
        if (!modules_.lookup(name, &module)) return false;

        CC_INSPECT(name);
        CC_INSPECT(module.source_);

        FileInfo srcInfo { module.source_ };
        if (!srcInfo) return false;

        cachePath = State::cachePath(name);
        FileInfo dstInfo { cachePrefix_ / *cachePath };
        if (dstInfo && srcInfo.lastModified() <= dstInfo.lastModified()) {
            bool dirty = false;
            for (const String &include: module.includes_) {
                CC_INSPECT(include);
                if (FileInfo{include}.lastModified() > dstInfo.lastModified()) {
                    dirty = true;
                    break;
                }
            }
            if (!dirty) return true;
        }

        guard.dismiss();

        Job job { plan.toolChain().interfaceUnitCompileCommand(plan, module.source_) };
        bool ok = job.run();
        scheduler.report(job);

        if (!ok) {
            ferr() << job.outputText() << nl;
        }

        return ok;
    }

    struct Module {
        Module() = default;
        Module(const String &source): source_{source} {}
        String source_;
        List<String> includes_;
    };

    mutable Mutex mutex_;
    Map<String, Module> modules_;
    const String cachePrefix_ { "gcm.cache" };
};

ImportManager::ImportManager()
{
    initOnce<State>();
}

void ImportManager::gatherModules(const String &importPrefix)
{
    me().gatherModules(importPrefix);
}

String ImportManager::moduleName(const String &importPrefix, const String &source)
{
    String suffix = source.shortFileSuffix();
    String name = source.copy(importPrefix.count() + 1, source.count() - suffix.count() - (suffix.count() > 0));
    name.replace('/', '.');
    return name;
}

bool ImportManager::registerModule(const String &name, const String &source, Out<String> otherSource)
{
    return me().registerModule(name, source, &otherSource);
}

void ImportManager::registerInclude(const String &name, const String &include)
{
    return me().registerInclude(name, include);
}

bool ImportManager::lookupModule(const String &name, Out<String> source)
{
    return me().lookupModule(name, &source);
}

String ImportManager::cachePrefix() const
{
    return me().cachePrefix_;
}

String ImportManager::cachePath(const String &name)
{
    return ImportManager::State::cachePath(name);
}

bool ImportManager::compileHeaderUnit(JobScheduler &scheduler, const BuildPlan &plan, const String &source, Out<String> cachePath) const
{
    return me().compileHeaderUnit(scheduler, plan, source, &cachePath);
}

bool ImportManager::compileInterfaceUnit(JobScheduler &scheduler, const BuildPlan &plan, const String &source, Out<String> cachePath) const
{
    return me().compileInterfaceUnit(scheduler, plan, source, &cachePath);
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
