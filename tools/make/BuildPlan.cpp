/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <fkit/stdio.h>
#include <fkit/Glob.h>
#include <fkit/Config.h>
#include "DependencyCache.h"
#include "GnuToolChain.h"
#include "JobScheduler.h"
#include "RecipeProtocol.h"
#include "BuildPlan.h"

namespace fmake
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

BuildPlan::BuildPlan(int argc, char **argv)
	: BuildShell(this),
	  AnalyseStage(this),
	  toolChain_(GnuToolChain::create()),
	  projectPath_("."),
	  buildMap_(BuildMap::create())
{
	initFlags();

	recipe_ = Config::read(argc, argv);
	if (recipe_->arguments()->size() > 0) {
		if (recipe_->arguments()->size() > 1)
			throw UserException("Processing multiple Recipe files at once is not supported");
		projectPath_ = recipe_->arguments()->at(0)->canonicalPath();
	}

	recipe_ = Config::read(projectPath_ + "/Recipe", recipeProtocol());
	recipe_ = Config::read(argc, argv, recipe_);
	readRecipe();

	buildMap_->insert(projectPath_, this);
}

BuildPlan::BuildPlan(String projectPath, BuildPlan *parentPlan)
	: BuildShell(this),
	  AnalyseStage(this),
	  toolChain_(parentPlan->toolChain_),
	  projectPath_(projectPath),
	  buildMap_(parentPlan->buildMap_)
{
	initFlags();

	recipe_ = Config::read(projectPath_ + "/Recipe", recipeProtocol());

	readRecipe(parentPlan);

	buildMap_->insert(projectPath_, this);
}

void BuildPlan::initFlags()
{
	buildComplete_ = false;
	testRunComplete_ = false;
	installComplete_ = false;
	uninstallComplete_ = false;
	cleanComplete_ = false;
	analyseResult_ = false;
	buildResult_ = false;
	testRunResult_ = 0;
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
	if (speedOptimizationLevel_ < 0) speedOptimizationLevel_ = toolChain_->defaultSpeedOptimizationLevel();
	if (sizeOptimizationLevel_ < 0) sizeOptimizationLevel_ = toolChain_->defaultSizeOptimizationLevel();

	includePaths_ = cast<StringList>(recipe_->value("include-path"));
	libraryPaths_ = cast<StringList>(recipe_->value("link-path"));
	libraries_ = cast<StringList>(recipe_->value("link"));

	if (!includePaths_) includePaths_ = StringList::create();
	if (!libraryPaths_) libraryPaths_ = StringList::create();
	if (!libraries_) libraries_ = StringList::create();

	installPrefix_ = recipe_->value("prefix");

	if (parentPlan) {
		options_ &= ~GlobalOptions;
		options_ |= parentPlan->options() & GlobalOptions;
		speedOptimizationLevel_ = parentPlan->speedOptimizationLevel();
		sizeOptimizationLevel_ = parentPlan->sizeOptimizationLevel();
		installPrefix_ = parentPlan->installPrefix_;
	}
}

int BuildPlan::run()
{
	readPrequisites();
	globSources();
	initModules();

	if (options_ & Bootstrap) {
		ferr(
			"#!/bin/sh -ex\n"
			"SOURCE=$1\n"
			"MACHINE=$(%%)\n"
		) << toolChain_->machineCommand();
	}

	if (!analyseStage()->run()) return 1;

	if (recipe_->value("clean")) {
		clean();
		return 0;
	}

	if (recipe_->value("uninstall")) {
		return uninstall() ? 0 : 3;
	}

	if (!build()) return 2;

	if (recipe_->value("test-run")) {
		int ret = testRun();
		if (ret != 0) return ret;
	}

	if (recipe_->value("install")) {
		if (!install()) return 2;
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

void BuildPlan::readPrequisites()
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
		plan->readPrequisites();
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
	sourcePrefix_ = buildMap_->commonPrefix('/')->canonicalPath();

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
		} ;
		h << topLevel->fileName();
		f << h->reverse()->join("_");
	}
	if (version_ != "") f << version_;
	if (options_ & Static)    f << "static";
	if (options_ & Debug)     f << "debug";
	if (options_ & Release)   f << "release";
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

bool BuildPlan::build()
{
	if (buildComplete_) return buildResult_;
	buildComplete_ = true;

	if ((options_ & Tests) && !(options_ & BuildTests)) return buildResult_ = true;

	for (int i = 0; i < prerequisites_->size(); ++i)
		if (!prerequisites_->at(i)->build()) return buildResult_ = false;

	if (options_ & Package) return buildResult_ = true;

	Ref<JobScheduler> compileScheduler;
	Ref<JobScheduler> linkScheduler;

	for (int i = 0; i < modules_->size(); ++i) {
		Module *module = modules_->at(i);
		bool dirty = module->dirty();
		if (options_ & Tools)
			dirty = dirty || !fileStatus(module->toolName())->exists();
		if (dirty) {
			Ref<Job> job = toolChain_->createCompileJob(this, module);
			Ref<Job> linkJob;
			if (options_ & Tools) linkJob = toolChain_->createLinkJob(this, module);
			if (options_ & Simulate) {
				ferr() << beautify(job->command()) << nl;
				if (linkJob)
					ferr() << beautify(linkJob->command()) << nl;
			}
			else {
				if (!compileScheduler) {
					compileScheduler = JobScheduler::create();
					compileScheduler->start();
				}
				compileScheduler->schedule(job);
				if (linkJob) {
					if (!linkScheduler) linkScheduler = JobScheduler::create();
					linkScheduler->schedule(linkJob);
				}
			}
		}
	}

	if (compileScheduler) {
		for (Ref<Job> job; compileScheduler->collect(&job);) {
			ferr() << beautify(job->command()) << nl;
			fout() << job->outputText();
			if (job->status() != 0) return buildResult_ = false;
		}
	}

	if (options_ & Tools) {
		if (linkScheduler) {
			linkScheduler->start();
			for (Ref<Job> job; linkScheduler->collect(&job);) {
				ferr() << beautify(job->command()) << nl;
				fout() << job->outputText();
				if (job->status() != 0) return buildResult_ = false;
			}
		}
		return buildResult_ = true;
	}

	Ref<FileStatus> productStatus = fileStatus(toolChain_->linkName(this));
	if (productStatus->exists() && *sources_ == *analyseStage()->previousSources()) {
		double productTime = productStatus->lastModified();
		bool dirty = false;
		for (int i = 0; i < modules_->size(); ++i) {
			Module *module = modules_->at(i);
			Ref<FileStatus> moduleStatus = fileStatus(module->modulePath());
			if (moduleStatus->lastModified() > productTime) {
				dirty = true;
				break;
			}
		}
		Ref<FileStatus> recipeStatus = fileStatus(recipe_->path());
		if (recipeStatus->exists()) {
			if (recipeStatus->lastModified() > productTime) dirty = true;
			for (int i = 0; i < prerequisites_->size(); ++i) {
				Ref<FileStatus> recipeStatus = fileStatus(prerequisites_->at(i)->recipe_->path());
				if (recipeStatus->lastModified() > productTime) {
					dirty = true;
					break;
				}
			}
		}
		if (!dirty) return buildResult_ = true;
	}

	return buildResult_ = toolChain_->link(this);
}

int BuildPlan::testRun()
{
	if (testRunComplete_) return testRunResult_;
	testRunComplete_ = true;

	for (int i = 0; i < prerequisites_->size(); ++i) {
		testRunResult_ = prerequisites_->at(i)->testRun();
		if (testRunResult_ != 0) return testRunResult_;
	}

	if (!(options_ & Tests)) return 0;

	Ref<JobScheduler> scheduler = JobScheduler::create();

	for (int i = 0; i < modules_->size(); ++i) {
		Module *module = modules_->at(i);
		scheduler->schedule(toolChain_->createTestJob(this, module));
	}

	for (Ref<Job> job; scheduler->collect(&job);) {
		ferr() << job->command() << nl;
		fout() << job->outputText();
		if (job->status() != 0)
			return testRunResult_ = job->status();
	}

	return testRunResult_ = 0;
}

bool BuildPlan::install()
{
	if (installComplete_) return true;
	installComplete_ = true;

	if (options_ & Tests) return true;

	for (int i = 0; i < prerequisites_->size(); ++i)
		if (!prerequisites_->at(i)->install()) return false;

	if (options_ & Package) return true;

	if (options_ & Tools) {
		for (int i = 0; i < modules_->size(); ++i) {
			if (!toolChain_->install(this, modules_->at(i)))
				return false;
		}
		return true;
	}

	return toolChain_->install(this);
}

bool BuildPlan::uninstall()
{
	if (uninstallComplete_) return true;
	uninstallComplete_ = true;

	if (options_ & Tests) return true;

	for (int i = 0; i < prerequisites_->size(); ++i)
		if (!prerequisites_->at(i)->uninstall()) return false;

	if (options_ & Package) return true;

	if (options_ & Tools) {
		for (int i = 0; i < modules_->size(); ++i) {
			if (!toolChain_->uninstall(this, modules_->at(i)))
				return false;
		}
		return true;
	}

	return toolChain_->uninstall(this);
}

void BuildPlan::clean()
{
	if (cleanComplete_) return;
	cleanComplete_ = true;

	for (int i = 0; i < prerequisites_->size(); ++i)
		prerequisites_->at(i)->clean();

	if (options_ & Package) return;

	toolChain_->clean(this);
	unlink(DependencyCache::cachePath(this));
	rmdir(modulePath_);
}

} // namespace fmake
