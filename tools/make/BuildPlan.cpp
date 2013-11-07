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
#include <flux/Config.h>
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
	recipe_ = Config::read(argc, argv);
	if (recipe_->arguments()->size() > 0) {
		if (recipe_->arguments()->size() > 1)
			throw UserException("Processing multiple Recipe files at once is not supported");
		projectPath_ = recipe_->arguments()->at(0)->canonicalPath();
	}

	recipe_ = Config::read(projectPath_ + "/Recipe", recipeProtocol());
	recipe_ = Config::read(argc, argv, recipe_);
	readRecipe();

	toolChain_ = GnuToolChain::create(recipe_->value("compiler"));
	if (speedOptimizationLevel_ < 0) speedOptimizationLevel_ = toolChain_->defaultSpeedOptimizationLevel();
	if (sizeOptimizationLevel_ < 0) sizeOptimizationLevel_ = toolChain_->defaultSizeOptimizationLevel();

	buildMap_->insert(projectPath_, this);
}

BuildPlan::BuildPlan(String projectPath, BuildPlan *parentPlan)
	: toolChain_(parentPlan->toolChain_),
	  projectPath_(projectPath),
	  concurrency_(parentPlan->concurrency_),
	  buildMap_(parentPlan->buildMap_),
	  FLUXMAKE_BUILDPLAN_COMPONENTS_INIT
{
	recipe_ = Config::read(projectPath_ + "/Recipe", recipeProtocol());

	readRecipe(parentPlan);
	if (speedOptimizationLevel_ < 0) speedOptimizationLevel_ = toolChain_->defaultSpeedOptimizationLevel();
	if (sizeOptimizationLevel_ < 0) sizeOptimizationLevel_ = toolChain_->defaultSizeOptimizationLevel();

	buildMap_->insert(projectPath_, this);
}

void BuildPlan::readRecipe(BuildPlan *parentPlan)
{
	name_ = recipe_->value("name");
	version_ = recipe_->value("version");
	options_ = 0;
	speedOptimizationLevel_ = -1;
	sizeOptimizationLevel_ = -1;

	if (recipe_->className() == "Application")  options_ |= Application;
	else if (recipe_->className() == "Library") options_ |= Library;
	else if (recipe_->className() == "Tools")   options_ |= Tools;
	else if (recipe_->className() == "Tests")   options_ |= Tools | Tests;
	else if (recipe_->className() == "Package") options_ |= Package;

	if (recipe_->value("debug"))     options_ |= Debug;
	if (recipe_->value("release"))   options_ |= Release;
	if (recipe_->value("static"))    options_ |= Static;
	if (recipe_->value("simulate"))  options_ |= Simulate;
	if (recipe_->value("blindfold")) options_ |= Blindfold;
	if (recipe_->value("bootstrap")) options_ |= Bootstrap | Simulate | Blindfold;
	if (recipe_->value("test"))      options_ |= BuildTests;
	if (recipe_->value("test-run"))  options_ |= BuildTests;
	if (recipe_->value("clean"))     options_ |= BuildTests;
	if (recipe_->value("verbose"))   options_ |= Verbose;

	if (recipe_->value("optimize-speed")) options_ |= OptimizeSpeed;
	if (recipe_->value("optimize-size"))  options_ |= OptimizeSize;
	if (recipe_->value("optimize-debug")) options_ |= OptimizeDebug;

	speedOptimizationLevel_ = recipe_->value("speed-optimization-level", -1);
	sizeOptimizationLevel_ = recipe_->value("size-optimization-level", -1);

	includePaths_ = cast<StringList>(recipe_->value("include-path"));
	libraryPaths_ = cast<StringList>(recipe_->value("link-path"));
	libraries_ = cast<StringList>(recipe_->value("link"));
	if (!includePaths_) includePaths_ = StringList::create();
	if (!libraryPaths_) libraryPaths_ = StringList::create();
	if (!libraries_) libraries_ = StringList::create();

	installPrefix_ = recipe_->value("prefix");

	customCompileFlags_ = cast<StringList>(recipe_->value("compile-flags"));
	customLinkFlags_ = cast<StringList>(recipe_->value("link-flags"));
	if (!customCompileFlags_) customCompileFlags_ = StringList::create();
	if (!customLinkFlags_) customLinkFlags_ = StringList::create();
	if (options_ & Debug) {
		if (recipe_->contains("debug-compile-flags")) {
			Ref<StringList> h = cast<StringList>(recipe_->value("debug-compile-flags"));
			if (h) for (int i = 0; i < h->size(); ++i) customCompileFlags_->append(h->at(i));
		}
		if (recipe_->contains("debug-link-flags")) {
			Ref<StringList> h = cast<StringList>(recipe_->value("debug-link-flags"));
			if (h) for (int i = 0; i < h->size(); ++i) customLinkFlags_->append(h->at(i));
		}
	}
	else if (options_ & Release) {
		if (recipe_->contains("release-compile-flags")) {
			Ref<StringList> h = cast<StringList>(recipe_->value("release-compile-flags"));
			if (h) for (int i = 0; i < h->size(); ++i) customCompileFlags_->append(h->at(i));
		}
		if (recipe_->contains("release-link-flags")) {
			Ref<StringList> h = cast<StringList>(recipe_->value("release-link-flags"));
			if (h) for (int i = 0; i < h->size(); ++i) customLinkFlags_->append(h->at(i));
		}
	}

	concurrency_ = recipe_->value("concurrency");

	if (parentPlan) {
		options_ &= ~GlobalOptions;
		options_ |= parentPlan->options() & GlobalOptions;
		speedOptimizationLevel_ = parentPlan->speedOptimizationLevel();
		sizeOptimizationLevel_ = parentPlan->sizeOptimizationLevel();
		installPrefix_ = parentPlan->installPrefix_;
		concurrency_ = parentPlan->concurrency_;
	}
}

int BuildPlan::run()
{
	readSystemPrerequisites();
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

	if (systemPrerequisitesByName_) {
		if (!configureStage()->run()) return 1;
	}

	if (!analyseStage()->run()) return 1;

	if (recipe_->value("clean")) {
		return cleanStage()->run() ? 0 : 1;
	}

	if (recipe_->value("uninstall")) {
		return uninstallStage()->run() ? 0 : 1;
	}

	if (!compileLinkStage()->run()) return 1;

	if (recipe_->value("test-run")) {
		if (!testRunStage()->run())
			return testRunStage()->status();
	}

	if (recipe_->value("install")) {
		if (!installStage()->run()) return 1;
	}

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

void BuildPlan::readSystemPrerequisites()
{
	if (systemPrerequisitesByName_) return;

	if (!recipe_->hasChildren()) return;

	systemPrerequisitesByName_ = SystemPrerequisitesByName::create();

	for (int i = 0; i < recipe_->children()->size(); ++i) {
		YasonObject *object = recipe_->children()->at(i);
		if (object->className() != "SystemPrerequisite") continue;
		Ref<SystemPrerequisite> p = SystemPrerequisite::read(object);
		Ref<SystemPrerequisiteList> l;
		if (!systemPrerequisitesByName_->lookup(p->name(), &l))
			systemPrerequisitesByName_->insert(p->name(), l = SystemPrerequisiteList::create());
		l->append(p);
	}
}

void BuildPlan::readPrerequisites()
{
	if (prerequisites_) return;

	if ((options_ & Tests) && !(options_ & BuildTests)) return;

	prerequisites_ = BuildPlanList::create();

	StringList *prerequisitePaths = cast<StringList>(recipe_->value("use"));

	for (int i = 0; i < prerequisitePaths->size(); ++i) {
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
		}
		plan->readPrerequisites();
		prerequisites_->append(plan);
	}
}

void BuildPlan::globSources()
{
	if (sources_) return;

	if ((options_ & Tests) && !(options_ & BuildTests)) return;

	sources_ = StringList::create();
	if (recipe_->contains("source")) {
		StringList *sourcePatterns = cast<StringList>(recipe_->value("source"));
		for (int i = 0; i < sourcePatterns->size(); ++i) {
			Ref<Glob> glob = Glob::open(sourcePath(sourcePatterns->at(i)));
			for (String path; glob->read(&path);)
				sources_->append(path);
		}
	}
	sources_ = sources_->sort();
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
	if (options_ & Static) f << "static";
	if (options_ & Debug) f << "debug";
	if (options_ & Release) f << "release";
	if (options_ & OptimizeSpeed) {
		Format h;
		h << "optimize" << "speed" << speedOptimizationLevel_;
		f << h->join("-");
	}
	if (options_ & OptimizeSize) {
		Format h;
		h << "optimize" << "size" << sizeOptimizationLevel_;
		f << h->join("-");
	}
	if (options_ & Bootstrap)
		f << "$MACHINE";
	else
		f << toolChain_->machine();
	modulePath_ = f->join("-");

	for (int i = 0; i < prerequisites_->size(); ++i)
		prerequisites_->at(i)->initModules();
}

} // namespace fluxmake
