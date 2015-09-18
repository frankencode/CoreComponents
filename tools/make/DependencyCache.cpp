/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/Format>
#include <flux/File>
#include <flux/FileStatus>
#include <flux/meta/yason>
#include "BuildPlan.h"
#include "DependencyCache.h"

namespace fluxmake {

Ref<DependencyCache> DependencyCache::create(BuildPlan *plan)
{
    return new DependencyCache(plan);
}

String DependencyCache::cachePath(BuildPlan *plan)
{
    return plan->modulePath("DependencyCache");
}

DependencyCache::DependencyCache(BuildPlan *plan):
    buildPlan_(plan),
    cachePath_(cachePath(plan)),
    cache_(Cache::create()),
    previousSources_(StringList::create())
{
    File::establish(cachePath_);
    double cacheTime = File::status(cachePath_)->lastModified();

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

        MetaObject *yason = cast<MetaObject>(item->value());
        String command = yason->value("analyseCommand");
        String modulePath = yason->value("modulePath");
        Ref<StringList> dependencyPaths = cast<VariantList>(yason->value("dependencyPaths"))->toList<String>();
        String sourcePath = dependencyPaths->at(0);

        bool dirty = false;

        Ref<FileStatus> objectStatus = buildPlan_->shell()->fileStatus(modulePath);
        double objectTime = objectStatus->lastModified();

        for (int i = 0; i < dependencyPaths->count(); ++i)
        {
            Ref<FileStatus> sourceStatus = buildPlan_->shell()->fileStatus(dependencyPaths->at(i));
            if (!sourceStatus->exists()) {
                dirty = true;
                break;
            }
            double sourceTime = sourceStatus->lastModified();
            if (sourceTime > cacheTime) {
                dirty = true;
                break;
            }
            if (objectStatus->exists()) {
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
            Module::create(command, modulePath, dependencyPaths, !objectStatus->exists())
        );
    }
}

DependencyCache::~DependencyCache()
{
    Format text;
    String indent(4, ' ');
    text << "DependencyCache {\n";
    for (int i = 0; i < cache_->count(); ++i) {
        Cache::Item item = cache_->at(i);
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

StringList *DependencyCache::previousSources() const
{
    return previousSources_;
}

bool DependencyCache::lookup(String source, Ref<Module> *module)
{
    return cache_->lookup(source, module);
}

void DependencyCache::insert(String source, Module *module)
{
    cache_->insert(source, module);
}

} // namespace fluxmake
