/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include "DependencyCache.h"
#include "BuildPlan.h"
#include <cc/meta/yason>
#include <cc/File>
#include <cc/FileStatus>
#include <cc/Format>

namespace ccbuild {

Ref<DependencyCache> DependencyCache::create(BuildPlan *plan)
{
    return new DependencyCache(plan);
}

String DependencyCache::cachePath(BuildPlan *plan)
{
    return plan->modulePath("DependencyCache");
}

DependencyCache::DependencyCache(BuildPlan *plan):
    buildPlan_{plan},
    cachePath_{cachePath(plan)},
    cacheTime_{-1}
{
    File::establish(cachePath_);
    cacheTime_ = FileStatus{cachePath_}->lastModified();

    Ref<MetaObject> dependencyCache;
    try {
        dependencyCache = yason::parse(File::load(cachePath_));
    }
    catch (TextError &)
    {}
    if (!dependencyCache) return;
    if (dependencyCache->className() != "DependencyCache") return;

    for (int i = 0; i < dependencyCache->count(); ++i)
    {
        MetaObject::Item item = dependencyCache->at(i);

        previousSources_->append(item->key());

        if (!plan->sources()->contains(item->key())) continue;

        MetaObject *yason = Variant::cast<MetaObject *>(item->value());
        String command = yason->value("analyseCommand");
        String modulePath = yason->value("modulePath");
        StringList dependencyPaths = Variant::cast<StringList>(yason->value("dependencyPaths"));
            // here VariantList is implicitly converted to StringList: this is expensive!
            // \todo have proper prototypes for the cache files
        String sourcePath = dependencyPaths->at(0);

        bool dirty = false;

        FileStatus objectStatus = buildPlan_->shell()->fileStatus(modulePath);
        double objectTime = objectStatus->lastModified();

        for (String source: dependencyPaths)
        {
            FileStatus sourceStatus = buildPlan_->shell()->fileStatus(source);
            if (!sourceStatus->isValid()) {
                dirty = true;
                break;
            }
            double sourceTime = sourceStatus->lastModified();
            if (sourceTime > cacheTime_) {
                dirty = true;
                break;
            }
            if (objectStatus->isValid()) {
                if (sourceTime > objectTime /*|| cacheItemTime > objectTime*/) {
                    dirty = true;
                    break;
                }
            }
        }

        if (dirty) continue;

        if (command != buildPlan_->toolChain()->analyseCommand(plan, sourcePath)) continue;

        cache_->insert(
            item->key(),
            Module::create(command, modulePath, dependencyPaths, !objectStatus->isValid())
        );
    }
}

DependencyCache::~DependencyCache()
{
    Format text;
    String indent(4, ' ');
    text << "DependencyCache {\n";
    for (const auto &item: cache_) {
        String sourcePath = item->key();
        Module *module = item->value();
        text
            << indent << "\"" << sourcePath << "\": Module {\n"
            << indent << indent << "analyseCommand: \"" << module->analyseCommand() << "\"\n"
            << indent << indent << "modulePath: \"" << module->modulePath() << "\"\n"
            << indent << indent << "dependencyPaths: [\n";
        for (int i = 0, n = module->dependencyPaths()->count(); i < n; ++i) {
            text << indent << indent << indent << "\"" << module->dependencyPaths()->at(i);
            if (i != n - 1)
                text << "\",\n";
            else
                text << "\"\n";
        }
        text << indent << indent << "]\n"
            << indent << "}\n";
    }
    text << "}\n";
    File::save(cachePath_, text);
}

StringList DependencyCache::previousSources() const
{
    return previousSources_;
}

bool DependencyCache::lookup(const String &source, Ref<Module> *module)
{
    return cache_->lookup(source, module);
}

void DependencyCache::insert(const String &source, Module *module)
{
    cache_->insert(source, module);
}

} // namespace ccbuild
