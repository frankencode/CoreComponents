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
#include <cc/build/WaitableJob>
#include <cc/FileInfo>
#include <cc/File>
#include <cc/DirWalk>
#include <cc/Dir>
#include <cc/Map>
#include <cc/Set>
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
            otherSource = modules_.at(pos).value()->source_;
            return false;
        }
        return true;
    }

    void registerInclude(const String &name, const String &include)
    {
        Guard guard { mutex_ };

        Module module;
        if (modules_.lookup(name, &module)) {
            module->includes_.insert(include);
        }
    }

    bool lookupModule(const String &name, Out<String> source)
    {
        Guard guard { mutex_ };

        Module module;
        bool found = modules_.lookup(name, &module);
        if (found) source = module->source_;
        return found;
    }

    bool compileHeaderUnit(JobScheduler &scheduler, const BuildPlan &plan, const String &source, Out<String> cachePath)
    {
        cachePath = State::cachePath(source);

        Guard guard { mutex_ };

        Module module;
        if (!modules_.lookup(source, &module)) {
            module = Module{source};
            modules_.insert(source, module);
        }
        else if (module->job_) {
            guard.dismiss();
            module->job_.wait();
            guard.reinstate();
            return module->job_.status() == 0;
        }

        const String dstCachePath = cachePrefix_ / *cachePath;
        const String depCachePath = dstCachePath + ".d";

        module->includes_.deplete();
        for (const String &s: File::load(depCachePath).split('\n')) module->includes_.insert(s);

        CC_INSPECT(source);

        FileInfo srcInfo { module->source_ };
        if (!srcInfo) return false;

        FileInfo dstInfo { dstCachePath };
        bool dirty = !(dstInfo && srcInfo.lastModified() <= dstInfo.lastModified());
        if (!dirty) {
            for (const String &include: module->includes_) {
                if (FileInfo{include}.lastModified() > dstInfo.lastModified()) {
                    dirty = true;
                    break;
                }
            }
        }

        module->job_ = WaitableJob { plan.toolChain().headerUnitCompileCommand(plan, source) };

        guard.dismiss();

        bool ok = true;

        CC_INSPECT(dirty);

        if (dirty) {
            ok = module->job_.run();

            if (!ok) {
                fout() << module->job_.command() << nl;
                ferr() << module->job_.outputText() << nl;
            }
            else {
                scheduler.report(module->job_);

                File::save(depCachePath, module->includes_.toList().join('\n'));
            }
        }
        else {
            module->job_.notify();
        }

        return ok;
    }

    bool compileInterfaceUnit(JobScheduler &scheduler, const BuildPlan &plan, const String &name, Out<String> cachePath)
    {
        cachePath = State::cachePath(name);

        Guard guard { mutex_ };

        Module module;
        if (!modules_.lookup(name, &module)) return false;

        CC_INSPECT(name);
        CC_INSPECT(module->source_);

        if (module->job_) {
            guard.dismiss();
            module->job_.wait();
            guard.reinstate();
            return module->job_.status() == 0;
        }

        const String dstCachePath = cachePrefix_ / *cachePath;
        const String depCachePath = dstCachePath + ".d";

        module->includes_.deplete();
        for (const String &s: File::load(depCachePath).split('\n')) module->includes_.insert(s);

        FileInfo srcInfo { module->source_ };
        if (!srcInfo) return false;

        FileInfo dstInfo { dstCachePath };
        bool dirty = !(dstInfo && srcInfo.lastModified() <= dstInfo.lastModified());
        if (!dirty) {
            for (const String &include: module->includes_) {
                if (FileInfo{include}.lastModified() > dstInfo.lastModified()) {
                    dirty = true;
                    break;
                }
            }
        }

        module->job_ = WaitableJob { plan.toolChain().interfaceUnitCompileCommand(plan, module->source_) };

        guard.dismiss();

        bool ok = true;

        if (dirty) {
            ok = module->job_.run();

            if (!ok) {
                fout() << module->job_.command() << nl;
                ferr() << module->job_.outputText() << nl;
            }
            else {
                scheduler.report(module->job_);

                File::save(depCachePath, module->includes_.toList().join('\n'));
            }
        }
        else {
            module->job_.notify();
        }

        return ok;
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

    class Module final: public Object
    {
    public:
        Module() = default;

        Module(const String &source):
            Object{new State{source}}
        {}

        struct State final: public Object::State
        {
            explicit State(const String &source):
                source_{source}
            {}

            String source_;
            Set<String> includes_;
            WaitableJob job_;
        };

        State *operator->() { return &Object::me.as<State>(); }
        const State *operator->() const { return &Object::me.as<State>(); }
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

bool ImportManager::compileHeaderUnit(JobScheduler &scheduler, const BuildPlan &plan, const String &source, Out<String> cachePath)
{
    return me().compileHeaderUnit(scheduler, plan, source, &cachePath);
}

bool ImportManager::compileInterfaceUnit(JobScheduler &scheduler, const BuildPlan &plan, const String &source, Out<String> cachePath)
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
