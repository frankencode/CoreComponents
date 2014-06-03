/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <flux/stdio.h>
#include <flux/Glob.h>
#include <flux/Arguments.h>
#include <flux/ResourceContext.h>
#include <flux/yason.h>
#include "DependencyCache.h"
#include "GnuToolChain.h"
#include "JobScheduler.h"
#include "RecipeProtocol.h"
#include "BuildPlan.h"

namespace fluxmake
{

Ref<BuildPlan> BuildPlan::create(int argc, char **argv)
{
	return new BuildPlan(argc, argv);
}

Ref<BuildPlan> BuildPlan::create(String projectPath)
{
	BuildPlan *plan = 0;
	if (buildMap_->lookup(projectPath, &plan)) return plan;
	return new BuildPlan(projectPath, this);
}

#define FLUXMAKE_BUILDPLAN_COMPONENTS_INIT \
	shell_(this), \
	configureStage_(this), \
	analyseStage_(this), \
	compileLinkStage_(this), \
	testRunStage_(this), \
	installStage_(this), \
	uninstallStage_(this), \
	cleanStage_(this)

BuildPlan::BuildPlan(int argc, char **argv)
	: projectPath_("."),
	  concurrency_(-1),
	  buildMap_(BuildMap::create()),
	  FLUXMAKE_BUILDPLAN_COMPONENTS_INIT
{
	Ref<Arguments> arguments = Arguments::parse(argc, argv);
	StringList *items = arguments->items();

	if (items->size() > 0) {
		if (items->size() > 1)
			throw UsageError("Handling multiple source directories at once is not supported");
		projectPath_ = items->at(0)->canonicalPath();
	}

	ResourceContext context(recipePath());
	recipe_ = yason::parse(File::open(recipePath())->map(), recipeProtocol());
	arguments->validate(recipe_);
	arguments->override(recipe_);

	readRecipe();

	toolChain_ = GnuToolChain::create(compiler());
	if (optimize_ == "") optimize_ = toolChain_->defaultOptimization(this);

	buildMap_->insert(projectPath_, this);
}

BuildPlan::BuildPlan(String projectPath, BuildPlan *parentPlan)
	: toolChain_(parentPlan->toolChain_),
	  projectPath_(projectPath),
	  concurrency_(parentPlan->concurrency_),
	  buildMap_(parentPlan->buildMap_),
	  FLUXMAKE_BUILDPLAN_COMPONENTS_INIT
{
	recipe_ = yason::parse(File::open(recipePath())->map(), recipeProtocol());
	readRecipe(parentPlan);
	buildMap_->insert(projectPath_, this);
}

void BuildPlan::readRecipe(BuildPlan *parentPlan)
{
	name_ = recipe_->value("name");
	alias_ = recipe_->value("alias");
	version_ = recipe_->value("version");
	installPrefix_ = recipe_->value("prefix");

	options_ = 0;

	if (recipe_->className() == "Application")  options_ |= Application;
	else if (recipe_->className() == "Library") options_ |= Library;
	else if (recipe_->className() == "Tools")   options_ |= Tools;
	else if (recipe_->className() == "Tests")   options_ |= Tools | Tests;
	else if (recipe_->className() == "Package") options_ |= Package;

	if (recipe_->value("debug"))     options_ |= Debug;
	if (recipe_->value("release"))   options_ |= Release;
	if (recipe_->value("simulate"))  options_ |= Simulate;
	if (recipe_->value("blindfold")) options_ |= Blindfold;
	if (recipe_->value("bootstrap")) options_ |= Bootstrap | Simulate | Blindfold;
	if (recipe_->value("test"))      options_ |= BuildTests;
	if (recipe_->value("test-run"))  options_ |= BuildTests;
	if (recipe_->value("clean"))     options_ |= BuildTests;
	if (recipe_->value("verbose"))   options_ |= Verbose;

	concurrency_ = recipe_->value("concurrency");

	if (parentPlan) {
		options_ &= ~GlobalOptions;
		options_ |= parentPlan->options() & GlobalOptions;
		concurrency_ = parentPlan->concurrency_;
		installPrefix_ = parentPlan->installPrefix_;
	}

	BuildParameters::read(recipe_, this);

	if (recipe_->hasChildren()) {
		for (int i = 0; i < recipe_->children()->size(); ++i) {
			YasonObject *object = recipe_->children()->at(i);
			if (object->className() == "SystemPrerequisite") {
				Ref<SystemPrerequisite> p = SystemPrerequisite::read(object, this);
				Ref<SystemPrerequisiteList> l;
				if (!systemPrerequisitesByName_)
					systemPrerequisitesByName_ = SystemPrerequisitesByName::create();
				if (!systemPrerequisitesByName_->lookup(p->name(), &l))
					systemPrerequisitesByName_->insert(p->name(), l = SystemPrerequisiteList::create());
				l->append(p);
			}
			else if (object->className() == "Usage") {
				usage_ = BuildParameters::create();
				usage_->read(object, this);
				BuildParameters::readSpecific(usage_);
			}
		}
	}

	if (parentPlan) {
		optimize_ = parentPlan->optimize();
		linkStatic_ = parentPlan->linkStatic();
	}
}

int BuildPlan::run()
{
	readPrerequisites();
	globSources();
	initModules();

	if (options_ & Bootstrap) {
		fout(
			"#!/bin/sh -ex\n"
			"SOURCE=$1\n"
			"MACHINE=$(%%)\n"
		) << toolChain_->machineCommand();
	}

	if (!configureStage()->run()) return 1;
	if (!analyseStage()->run()) return 1;

	if (recipe_->value("clean")) return !cleanStage()->run();
	if (recipe_->value("uninstall")) return !uninstallStage()->run();

	if (!compileLinkStage()->run()) return 1;

	if (recipe_->value("test-run")) {
		if (!testRunStage()->run())
			return testRunStage()->status();
	}

	if (recipe_->value("install")) return !installStage()->run();

	return 0;
}

String BuildPlan::sourcePath(String source) const
{
	if (projectPath_ == ".") return source;
	return projectPath_ + "/" + source;
}

String BuildPlan::modulePath(String object) const
{
	return modulePath_ + "/" + object;
}

String BuildPlan::installPath(String relativeInstallPath) const
{
	return installPrefix_ + "/" + relativeInstallPath;
}

void BuildPlan::readPrerequisites()
{
	if (prerequisites_) return;

	prerequisites_ = BuildPlanList::create();

	if ((options_ & Tests) && !(options_ & BuildTests)) return;

	StringList *prerequisitePaths = cast<StringList>(recipe_->value("use"));

	for (int i = 0; i < prerequisitePaths->size(); ++i)
	{
		String path = prerequisitePaths->at(i);
		if (path->isRelativePath()) path = projectPath_ + "/" + path;
		path = path->canonicalPath();
		Ref<BuildPlan> plan = BuildPlan::create(path);

		if (plan->options() & Library) {
			path = path->reducePath();
			if (!includePaths_->contains(path))
				includePaths_->append(path);
			if (!libraryPaths_->contains("."))
				libraryPaths_->append(".");
			libraries_->append(plan->name());

			if (plan->usage()) BuildParameters::readSpecific(plan->usage());
		}
		plan->readPrerequisites();
		prerequisites_->append(plan);
	}
}


Ref<StringList> BuildPlan::globSources(StringList *pattern) const
{
	Ref<StringList> sources = StringList::create();
	for (int i = 0; i < pattern->size(); ++i) {
		Ref<Glob> glob = Glob::open(sourcePath(pattern->at(i)));
		for (String path; glob->read(&path);)
			sources->append(path);
	}
	sources = sources->sort();
	return sources;
}

void BuildPlan::globSources()
{
	if (sources_) return;

	if ((options_ & Tests) && !(options_ & BuildTests)) return;

	if (recipe_->contains("source"))
		sources_ = globSources(cast<StringList>(recipe_->value("source")));
	else
		sources_ = StringList::create();

	sourcePrefix_ = buildMap_->commonPrefix('/');
	if (sourcePrefix_ == "") sourcePrefix_ = projectPath_;
	else sourcePrefix_ = sourcePrefix_->canonicalPath();

	containsCPlusPlus_ = false;
	for (int i = 0; i < sources_->size(); ++i) {
		String suffix = sources_->at(i)->suffix();
		if (suffix == "cpp" || suffix == "cc" || suffix == "cxx" || suffix == "mm") {
			containsCPlusPlus_ = true;
			break;
		}
	}

	if (recipe_->contains("bundle"))
		bundle_ = globSources(cast<StringList>(recipe_->value("bundle")));
	else
		bundle_ = StringList::create();

	for (int i = 0; i < prerequisites_->size(); ++i)
		prerequisites_->at(i)->globSources();
}

void BuildPlan::initModules()
{
	if (modules_) return;

	if ((options_ & Tests) && !(options_ & BuildTests)) return;

	modules_ = ModuleList::create();

	Format f;
	f << ".modules";
	{
		Format h;
		String path = projectPath_->absolutePath();
		String topLevel = sourcePrefix_->absolutePath();
		while (path != topLevel) {
			h << path->fileName();
			path = path->reducePath();
		}
		h << topLevel->fileName();
		f << h->reverse()->join("_");
	}
	if (version_ != "") f << version_;
	if (linkStatic_) f << "static";
	if (options_ & Debug) f << "debug";
	if (options_ & Release) f << "release";
	if (optimize_ != "") f << "O" + optimize_;
	if (options_ & Bootstrap)
		f << "$MACHINE";
	else
		f << toolChain_->machine();
	modulePath_ = f->join("-");

	for (int i = 0; i < prerequisites_->size(); ++i)
		prerequisites_->at(i)->initModules();
}

} // namespace fluxmake
