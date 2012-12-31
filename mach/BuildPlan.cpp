#include <ftl/PrintDebug.hpp>
#include <ftl/File.hpp>
#include <ftl/Dir.hpp>
#include <ftl/Glob.hpp>
#include <ftl/Process.hpp>
#include <ftl/Config.hpp>
#include <ftl/System.hpp>
#include "DependencyCache.hpp"
#include "GccToolChain.hpp"
#include "JobScheduler.hpp"
#include "BuildPlan.hpp"

namespace mach
{

Ref<BuildPlan, Owner> BuildPlan::create(int argc, char **argv)
{
	return new BuildPlan(argc, argv);
}

Ref<BuildPlan, Owner> BuildPlan::create(Ref<ToolChain> toolChain, String projectPath, int globalOptions)
{
	Ref<BuildPlan> buildPlan;
	if (buildMap_->lookup(projectPath, &buildPlan)) return buildPlan;
	return new BuildPlan(toolChain, projectPath, this);
}

BuildPlan::BuildPlan(int argc, char **argv)
	: toolChain_(GccToolChain::create()),
	  projectPath_("."),
	  buildMap_(BuildMap::create()),
	  prepareComplete_(false),
	  analyseComplete_(false),
	  buildComplete_(false),
	  cleanComplete_(false),
	  distCleanComplete_(false),
	  buildResult_(false)
{
	recipe_ = Config::create();
	recipe_->read(argc, argv);
	if (recipe_->arguments()->length() > 0) {
		if (recipe_->arguments()->length() > 1)
			FTL_THROW(BuildPlanException, "Processing multiple Recipe files at once is not supported");
		projectPath_ = recipe_->arguments()->at(0)->canonicalPath();
	}
	recipe_->clear();
	recipe_->read(projectPath_ + "/Recipe");
	recipe_->read(argc, argv);
	readRecipe();

	buildMap_->insert(projectPath_, this);
}

BuildPlan::BuildPlan(Ref<ToolChain> toolChain, String projectPath, Ref<BuildPlan> parentPlan)
	: toolChain_(toolChain),
	  projectPath_(projectPath),
	  buildMap_(parentPlan->buildMap_),
	  prepareComplete_(false),
	  analyseComplete_(false),
	  buildComplete_(false),
	  cleanComplete_(false),
	  distCleanComplete_(false),
	  buildResult_(false)
{
	recipe_ = Config::create();
	recipe_->read(projectPath_ + "/Recipe");

	readRecipe(parentPlan);

	buildMap_->insert(projectPath_, this);
}

void BuildPlan::readRecipe(Ref<BuildPlan> parentPlan)
{
	name_ = recipe_->value("name");
	version_ = recipe_->value("version");
	options_ = 0;
	speedOptimizationLevel_ = -1;
	sizeOptimizationLevel_ = -1;

	if (recipe_->className() == "Application") options_ |= Application;
	else if (recipe_->className() == "Library") options_ |= Library;
	else if (recipe_->className() == "ToolSet") options_ |= ToolSet;
	else if (recipe_->className() == "Package") options_ |= Package;

	if (recipe_->flag("debug"))     options_ |= Debug;
	if (recipe_->flag("release"))   options_ |= Release;
	if (recipe_->flag("static"))    options_ |= Static;
	if (recipe_->flag("dry-run"))   options_ |= DryRun;
	if (recipe_->flag("blindfold")) options_ |= Blindfold;
	if (recipe_->flag("bootstrap")) options_ |= Bootstrap | DryRun | Blindfold;
	if (recipe_->flag("verbose"))   options_ |= Verbose;

	if (recipe_->contains("optimize-speed")) {
		options_ |= OptimizeSpeed;
		Variant h = recipe_->value("optimize-speed");
		if (h.type() == Variant::IntType)
			speedOptimizationLevel_ = h;
		else
			speedOptimizationLevel_ = toolChain_->defaultSpeedOptimizationLevel();
	}
	if (recipe_->contains("optimize-size")) {
		options_ |= OptimizeSize;
		Variant h = recipe_->value("optimize-size");
		if (h.type() == Variant::IntType)
			sizeOptimizationLevel_ = h;
		else
			sizeOptimizationLevel_ = toolChain_->defaultSizeOptimizationLevel();
	}

	if (recipe_->contains("include-path"))
		includePaths_ = Ref<VariantList>(recipe_->value("include-path"))->toList<String>();
	else
		includePaths_ = StringList::create();

	if (recipe_->contains("link-path"))
		libraryPaths_ = Ref<VariantList>(recipe_->value("link-path"))->toList<String>();
	else
		libraryPaths_ = StringList::create();

	if (recipe_->contains("link"))
		libraries_ = Ref<VariantList>(recipe_->value("link"))->toList<String>();
	else
		libraries_ = StringList::create();

	if (parentPlan) {
		options_ &= ~GlobalOptions;
		options_ |= (parentPlan->options() & GlobalOptions);
		speedOptimizationLevel_ = parentPlan->speedOptimizationLevel();
		sizeOptimizationLevel_ = parentPlan->sizeOptimizationLevel();
	}
}

int BuildPlan::run()
{
	if (recipe_->flag("h") || recipe_->flag("help")) {
		print("no help, yet...\n");
		return 0;
	}

	prepare();

	if (options_ & Bootstrap) {
		printTo(error(),
			"#!/bin/sh -ex\n"
			"SOURCE=$1\n"
			"MACHINE=$(%%)\n",
			toolChain_->machineCommand()
		);
	}

	analyse();

	if (recipe_->flag("c") || recipe_->flag("clean")) {
		clean();
		return 0;
	}

	return build() ? 0 : 1;
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

String BuildPlan::beautifyCommand(String command)
{
	if (options_ & Bootstrap)
		return command->replace(String(" ") + sourcePrefix_, " $SOURCE");
	return command;
}

String BuildPlan::runAnalyse(String command)
{
	if (options_ & Verbose) error()->writeLine(beautifyCommand(command));
	return Process::start(command, Process::ForwardOutput)->rawOutput()->readAll();
}

bool BuildPlan::runBuild(String command)
{
	error()->writeLine(beautifyCommand(command));
	if (options_ & DryRun) return true;
	return Process::start(command)->wait() == 0;
}

bool BuildPlan::mkdir(String path)
{
	if (!fileStatus(path)->exists())
		printTo(error(), "mkdir -p %%\n", path);
	if (options_ & DryRun) return true;
	return Dir::establish(path);
}

bool BuildPlan::rmdir(String path)
{
	if (fileStatus(path)->exists())
		printTo(error(), "rmdir %%\n", path);
	if (options_ & DryRun) return true;
	return Dir::unlink(path);
}

bool BuildPlan::symlink(String path, String newPath)
{
	printTo(error(), "ln -sf %% %%\n", path, newPath);
	if (options_ & DryRun) return true;
	File::unlink(newPath);
	return File::symlink(path, newPath);
}

bool BuildPlan::unlink(String path)
{
	if (File::unresolvedStatus(path)->exists()) {
		printTo(error(), "rm %%\n", path);
		if (options_ & DryRun) return true;
		return File::unlink(path);
	}
	return true;
}

Ref<FileStatus, Owner> BuildPlan::fileStatus(String path)
{
	if (options_ & Blindfold) return FileStatus::read();
	return FileStatus::read(path);
}

void BuildPlan::prepare()
{
	if (prepareComplete_) return;
	prepareComplete_ = true;

	prequisites_ = BuildPlanList::create();

	Ref<StringList, Owner> prequisitePaths;
	if (recipe_->contains("use"))
		prequisitePaths = Ref<VariantList>(recipe_->value("use"))->toList<String>();
	else
		prequisitePaths = StringList::create();

	for (int i = 0; i < prequisitePaths->length(); ++i) {
		String path = prequisitePaths->at(i);
		if (path->isRelativePath()) path = projectPath_ + "/" + path;
		path = path->canonicalPath();
		Ref<BuildPlan, Owner> buildPlan = BuildPlan::create(toolChain_, path, options_ & GlobalOptions);
		if (buildPlan->options() & Library) {
			path = path->reducePath();
			if (!includePaths_->contains(path))
				includePaths_->append(path);
			if (!libraryPaths_->contains("."))
				libraryPaths_->append(".");
			libraries_->append(buildPlan->name());
		}
		buildPlan->prepare();
		prequisites_->append(buildPlan);
	}

	sources_ = StringList::create();
	if (recipe_->contains("source")) {
		Ref<VariantList> sourcePatterns = recipe_->value("source");
		for (int i = 0; i < sourcePatterns->length(); ++i) {
			Ref<Glob, Owner> glob = Glob::open(sourcePath(sourcePatterns->at(i)));
			for (String path; glob->read(&path);)
				sources_->append(path);
		}
	}
}

void BuildPlan::analyse()
{
	if (analyseComplete_) return;
	analyseComplete_ = true;

	sourcePrefix_ = buildMap_->commonPrefix()->canonicalPath();

	{
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
	}

	for (int i = 0; i < prequisites_->length(); ++i)
		prequisites_->at(i)->analyse();

	if (options_ & Package) return;

	mkdir(modulePath_);

	modules_ = ModuleList::create();
	Ref<JobList, Owner> jobList = JobList::create();

	Ref<DependencyCache, Owner> dependencyCache = DependencyCache::create(this);
	for (int i = 0; i < sources_->length(); ++i) {
		/*Ref<Module, Owner> module;
		if (dependencyCache->lookup(sources_->at(i), &module))
			modules_->append(module);
		else*/
			jobList->append(toolChain_->createAnalyseJob(this, sources_->at(i)));
	}

	if (jobList->length() == 0) return;

	Ref<JobScheduler, Owner> scheduler = JobScheduler::start(jobList);

	for (Ref<Job, Owner> job; scheduler->collect(&job);) {
		if (options_ & Verbose) {
			error()->writeLine(beautifyCommand(job->command()));
			error()->writeLine(job->outputText());
		}
		modules_->append(toolChain_->finishAnalyseJob(this, job));
	}

	// FIXME: missing error handling
}

bool BuildPlan::build()
{
	if (buildComplete_) return buildResult_;
	buildComplete_ = true;

	for (int i = 0; i < prequisites_->length(); ++i)
		if (!prequisites_->at(i)->build()) return buildResult_ = false;

	if (options_ & Package) return buildResult_ = true;

	for (int i = 0; i < modules_->length(); ++i) {
		Ref<Module> module = modules_->at(i);
		bool dirty = module->dirty();
		if (options_ & ToolSet)
			dirty = dirty || !fileStatus(module->toolName())->exists();
		if (dirty) {
			if (!toolChain_->compile(this, module)) return buildResult_ = false;
		}
	}

	if (options_ & ToolSet) return buildResult_ = true;

	Ref<FileStatus, Owner> targetStatus = fileStatus(toolChain_->linkPath(this));
	if (targetStatus->exists()) {
		Time targetTime = targetStatus->lastModified();
		bool targetDirty = false;
		for (int i = 0; i < modules_->length(); ++i) {
			Ref<Module> module = modules_->at(i);
			if (fileStatus(module->modulePath())->lastModified() > targetTime) {
				targetDirty = true;
				break;
			}
		}
		Ref<FileStatus, Owner> recipeStatus = fileStatus(recipe_->path());
		if (recipeStatus->exists()) {
			if (recipeStatus->lastModified() > targetTime) targetDirty = true;
		}
		if (!targetDirty) return buildResult_ = true;
	}

	return buildResult_ = toolChain_->link(this);
}

void BuildPlan::clean()
{
	if (cleanComplete_) return;
	cleanComplete_ = true;

	for (int i = 0; i < prequisites_->length(); ++i)
		prequisites_->at(i)->clean();

	if (options_ & Package) return;

	toolChain_->clean(this);
	unlink(DependencyCache::cachePath(this));
	rmdir(modulePath_);
}

} // namespace mach
