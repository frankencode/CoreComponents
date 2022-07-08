/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/build/DependencyCache>
#include <cc/build/Module>
#include <cc/FileStatus>
#include <cc/File>
#include <cc/Format>
#include <cc/yason>

namespace cc::build {

struct DependencyCache::State: public Object::State
{
    State(const BuildPlan &plan):
        cachePath_{plan.modulePath("DependencyCache")}
    {
        File::establish(cachePath_);
        cacheTime_ = FileStatus{cachePath_}.lastModified();

        MetaObject dependencyCache;
        try {
            Variant value = yasonParse(File::load(cachePath_));
            if (value.is<MetaObject>()) dependencyCache = value.to<MetaObject>();
        }
        catch (TextError &)
        {}

        if (!dependencyCache) return;
        if (dependencyCache.className() != "DependencyCache") return;

        for (const auto &item: dependencyCache.members())
        {
            previousSources_.append(item.key());

            if (!plan.sources().contains(item.key())) continue;

            MetaObject object = item.value().to<MetaObject>();
            String command = object("analyseCommand").to<String>();
            String modulePath = object("modulePath").to<String>();
            List<String> dependencyPaths;
            {
                List<Variant> vl = object("dependencyPaths").to<List<Variant>>();
                for (const Variant &item: vl) dependencyPaths.append(item.to<String>());
                // Here List<Variant> is implicitly converted to List<String> -- this is expensive!
                //! \todo Have proper prototypes for the cache files.
            }

            String sourcePath = dependencyPaths.at(0);

            bool dirty = false;

            FileStatus objectStatus = plan.shell().fileStatus(modulePath);
            double objectTime = objectStatus.lastModified();

            for (const String &source: dependencyPaths)
            {
                FileStatus sourceStatus = plan.shell().fileStatus(source);
                if (!sourceStatus) {
                    dirty = true;
                    break;
                }
                double sourceTime = sourceStatus.lastModified();
                if (sourceTime > cacheTime_) {
                    dirty = true;
                    break;
                }
                if (objectStatus) {
                    if (sourceTime > objectTime) {
                        dirty = true;
                        break;
                    }
                }
            }

            if (dirty) continue;

            if (command != plan.toolChain().analyseCommand(plan, sourcePath)) continue;

            cache_.insert(
                item.key(),
                Module{command, modulePath, dependencyPaths, !objectStatus}
            );
        }
    }

    ~State()
    {
        String indent = String::allocate(4, ' ');

        Format text;
        text << "DependencyCache {\n";

        for (const auto &item: cache_)
        {
            const String &sourcePath = item.key();
            const Module &module = item.value();
            text
                << indent << "\"" << sourcePath << "\": Module {\n"
                << indent << indent << "analyseCommand: \"" << module.analyseCommand() << "\"\n"
                << indent << indent << "modulePath: \"" << module.modulePath() << "\"\n"
                << indent << indent << "dependencyPaths: [\n";

            const List<String> &dependencyPaths = module.dependencyPaths();
            for (const String &path: dependencyPaths) {
                text << indent << indent << indent << "\"" << path;
                if (!dependencyPaths.last(path))
                    text << "\",\n";
                else
                    text << "\"\n";
            }

            text
                << indent << indent << "]\n"
                << indent << "}\n";
        }
        text << "}\n";

        File::save(cachePath_, text);
    }

    String cachePath_;
    double cacheTime_ { -1 };
    Map<String, Module> cache_;
    List<String> previousSources_;
};

DependencyCache::DependencyCache(const BuildPlan &plan):
    Object{new State{plan}}
{}

double DependencyCache::cacheTime() const
{
    return me().cacheTime_;
}

List<String> DependencyCache::previousSources() const
{
    return me().previousSources_;
}

bool DependencyCache::lookup(const String &source, Out<Module> module)
{
    return me().cache_.lookup(source, &module);
}

void DependencyCache::insert(const String &source, const Module &module)
{
    me().cache_.insert(source, module);
}

const DependencyCache::State &DependencyCache::me() const
{
    return Object::me.as<State>();
}

DependencyCache::State &DependencyCache::me()
{
    return Object::me.as<State>();
}

} // namespace cc::build
