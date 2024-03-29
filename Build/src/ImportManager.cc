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
#include <cc/File>
#include <cc/DirWalk>
#include <cc/Dir>
#include <cc/Map>
#include <cc/Set>
#include <cc/Mutex>
#include <cc/Guard>
#include <cc/debugging>

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
            registerModule(moduleName(importPrefix, source), Unit{Unit::Type::SystemModule, source});
        }
    }

    bool registerModule(const String &name, const Unit &module, Out<Unit> other = None{})
    {
        Guard guard { mutex_ };

        Locator pos;
        bool ok = modules_.insert(name, module, &pos);
        if (!ok) other = modules_.at(pos).value();
        return ok;
    }

    bool lookupModule(const String &name, Out<Unit> module)
    {
        Guard guard { mutex_ };

        return modules_.lookup(name, &module);
    }

    bool compileHeaderUnit(JobScheduler &scheduler, const BuildPlan &plan, const String &import, Out<String> cachePath)
    {
        cachePath = State::cachePath(import);

        Guard guard { mutex_ };

        Unit module;
        if (!modules_.lookup(import, &module)) {
            module = Unit{Unit::Type::SystemModule, import};
            modules_.insert(import, module);
        }
        else if (module->job_) {
            guard.dismiss();
            module->job_.wait();
            guard.reinstate();
            return module->job_.status() == 0;
        }

        const String dstCachePath = cachePrefix_ / *cachePath;

        List<String> includes = File::load(dstCachePath + ".include").split('\n');
        List<String> imports = File::load(dstCachePath + ".import").split('\n');

        CC_INSPECT(plan.projectPath());
        CC_INSPECT(import);

        FileInfo srcInfo { module->source_ };
        if (!srcInfo) return false;

        FileInfo dstInfo { dstCachePath };
        bool dirty = !(dstInfo && srcInfo.lastModified() <= dstInfo.lastModified());
        if (!dirty) {
            for (const String &include: includes) {
                if (FileInfo{include}.lastModified() > dstInfo.lastModified()) {
                    dirty = true;
                    break;
                }
            }
        }

        module->job_ = Job { plan.toolChain().headerUnitCompileCommand(plan, import) };

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
            }
        }
        else {
            module->job_.notify();
        }

        return ok;
    }

    bool compileInterfaceUnit(JobScheduler &scheduler, const BuildPlan &plan, const String &import, Out<String> cachePath, Out<String> source)
    {
        cachePath = State::cachePath(import);

        Guard guard { mutex_ };

        Unit module;
        if (!modules_.lookup(import, &module)) return false;

        CC_INSPECT(import);
        CC_INSPECT(module->source_);

        source = module->source_;

        if (module->job_) {
            guard.dismiss();
            module->job_.wait();
            guard.reinstate();
            return module->job_.status() == 0;
        }

        const String dstCachePath = cachePrefix_ / *cachePath;

        List<String> includes = File::load(dstCachePath + ".include").split('\n');
        List<String> imports = File::load(dstCachePath + ".import").split('\n');

        FileInfo srcInfo { module->source_ };
        if (!srcInfo) return false;

        FileInfo dstInfo { dstCachePath };
        bool dirty = !(dstInfo && srcInfo.lastModified() <= dstInfo.lastModified());
        if (!dirty) {
            for (const String &include: includes) {
                if (FileInfo{include}.lastModified() > dstInfo.lastModified()) {
                    dirty = true;
                    break;
                }
            }
        }

        module->job_ = Job { plan.toolChain().interfaceUnitCompileCommand(plan, module->source_) };

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

                // File::save(depCachePath, module->includes_.toList().join('\n')); // FIXME
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

    mutable Mutex mutex_;
    Map<String, Unit> modules_;
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

bool ImportManager::registerModule(const String &name, const Unit &module, Out<Unit> other)
{
    return me().registerModule(name, module, &other);
}

bool ImportManager::lookupModule(const String &name, Out<Unit> module)
{
    return me().lookupModule(name, &module);
}

String ImportManager::cachePrefix() const
{
    return me().cachePrefix_;
}

String ImportManager::cachePath(const String &name)
{
    return ImportManager::State::cachePath(name);
}

bool ImportManager::compileHeaderUnit(JobScheduler &scheduler, const BuildPlan &plan, const String &import, Out<String> cachePath)
{
    return me().compileHeaderUnit(scheduler, plan, import, &cachePath);
}

bool ImportManager::compileInterfaceUnit(JobScheduler &scheduler, const BuildPlan &plan, const String &import, Out<String> cachePath, Out<String> source)
{
    return me().compileInterfaceUnit(scheduler, plan, import, &cachePath, &source);
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
