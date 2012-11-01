#include "PrintDebug.hpp"
#include "File.hpp"
#include "Process.hpp"
#include "Glob.hpp"
#include "Config.hpp"
#include "DependencyCache.hpp"
#include "ToolChain.hpp"
#include "BuildPlan.hpp"

namespace ftl
{

class PathScope {
public:
	PathScope(String path, bool verbose = true)
		: cwd_(path),
		  verbose_(verbose),
		  cwdSaved_(Process::cwd()),
		  redundant_((cwdSaved_ == cwd_->makeAbsolutePath()) || (cwd_ == "."))
	{
		if (redundant_) return;
		if (verbose_) printTo(error(), "pushd %%\n", cwd_);
		Process::cd(cwd_);
	}
	~PathScope() {
		if (redundant_) return;
		if (verbose_) printTo(error(), "popd\n");
		Process::cd(cwdSaved_);
	}
private:
	String cwd_;
	bool verbose_;
	String cwdSaved_;
	bool redundant_;
};

Ref<BuildPlan, Owner> BuildPlan::newInstance(String projectPath, int globalOptions)
{
	return new BuildPlan(projectPath, globalOptions);
}

BuildPlan::BuildPlan(String projectPath, int globalOptions)
	:  projectPath_(projectPath)
{
	recipe_ = Config::newInstance();
	recipe_->read(projectPath + "/Recipe");

	options_ = optionsFromRecipe(recipe_);

	if (globalOptions != Unspecified) {
		options_ &= ~GlobalOptions;
		options_ |= globalOptions;
	}

	name_ = recipe_->value("name");
	version_ = recipe_->value("version");

	if (recipe_->contains("includePath"))
		includePaths_ = Ref<VariantList>(recipe_->value("linkPath"))->toList<String>();
	else
		includePaths_ = StringList::newInstance();

	if (recipe_->contains("linkPath"))
		libraryPaths_ = Ref<VariantList>(recipe_->value("linkPath"))->toList<String>();
	else
		libraryPaths_ = StringList::newInstance();

	if (recipe_->contains("link"))
		libraries_ = Ref<VariantList>(recipe_->value("link"))->toList<String>();
	else
		libraries_ = StringList::newInstance();
}

String BuildPlan::runAnalyse(String command)
{
	if (options_ & Verbose) printTo(error(), command);
	return Process::start(command, Process::ForwardOutput)->rawOutput()->readAll();
}

bool BuildPlan::runBuild(String command)
{
	error()->writeLine(command);
	if (options_ & DryRun) return true;
	return Process::start(command)->wait() == 0;
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
	if (fileStatus(path)->exists()) {
		printTo(error(), "rm %%\n", path);
		if (options_ & DryRun) return true;
		return File::unlink(path);
	}
	return true;
}

Ref<FileStatus, Owner> BuildPlan::fileStatus(String path)
{
	if (options_ & Blindfold) return FileStatus::newInstance();
	return FileStatus::newInstance(path);
}

int BuildPlan::run(Ref<ToolChain> toolChain, int argc, char **argv)
{
	recipe_->read(argc, argv);

	if (recipe_->flag("h") || recipe_->flag("help")) {
		print("no help, yet...\n");
		return 0;
	}

	options_ = optionsFromRecipe(recipe_);
	analyse(toolChain);

	if (recipe_->flag("c") || recipe_->flag("clean")) {
		clean(toolChain);
		return 0;
	}
	if (recipe_->flag("dist-clean")) {
		distClean(toolChain);
		return 0;
	}

	return build(toolChain) ? 0 : 1;
}

void BuildPlan::analyse(Ref<ToolChain> toolChain)
{
	if (sources_) return;

	bool verbose = options_ & Verbose;
	PathScope pathScope(projectPath_, verbose);

	prequisites_ = BuildPlanList::newInstance();

	Ref<StringList, Owner> prequisitePaths;
	if (recipe_->contains("use"))
		prequisitePaths = Ref<VariantList>(recipe_->value("use"))->toList<String>();
	else
		prequisitePaths = StringList::newInstance();

	for (int i = 0; i < prequisitePaths->length(); ++i) {
		String path = prequisitePaths->at(i);
		PathScope cwd(path, verbose);
		Ref<Config, Owner> recipe = Config::newInstance();
		recipe->read("Recipe");
		if (recipe->className() == "Library") {
			String parentPath = path->reducePath();
			if (!includePaths_->contains(parentPath))
				includePaths_->append(parentPath);
			libraryPaths_->append(path);
			libraries_->append(recipe->value("name"));
		}
		Ref<BuildPlan, Owner> prequisite = BuildPlan::newInstance(path, options_ & GlobalOptions);
		prequisite->analyse(toolChain);
		prequisites_->append(prequisite);
	}

	sources_ = StringList::newInstance();
	Ref<VariantList> sourcePatterns = recipe_->value("source");
	Ref<DirEntry, Owner> entry = DirEntry::newInstance();
	for (int i = 0; i < sourcePatterns->length(); ++i) {
		Ref<Glob, Owner> glob = Glob::newInstance(sourcePatterns->at(i));
		while (glob->read(entry))
			sources_->append(entry->path());
	}

	modules_ = ModuleList::newInstance(sources_->length());
	Ref<DependencyCache, Owner> dependencyCache = DependencyCache::newInstance(this, toolChain, sources_, options_, includePaths_);
	for (int i = 0; i < sources_->length(); ++i)
		modules_->set(i, dependencyCache->analyse(sources_->at(i), options_, includePaths_));
}

bool BuildPlan::build(Ref<ToolChain> toolChain)
{
	PathScope pathScope(projectPath_);

	for (int i = 0; i < prequisites_->length(); ++i)
		if (!prequisites_->at(i)->build(toolChain)) return false;

	for (int i = 0; i < modules_->length(); ++i) {
		Ref<Module> module = modules_->at(i);
		if (module->dirty()) {
			if (!toolChain->compile(this, module, options_, includePaths_)) return false;
		}
	}

	Ref<FileStatus, Owner> targetStatus = fileStatus(toolChain->linkPath(name_, version_, options_));
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
		if (!targetDirty) return true;
	}

	return toolChain->link(this, modules_, libraryPaths_, libraries_, name_, version_, options_);
}

void BuildPlan::clean(Ref<ToolChain> toolChain)
{
	for (int i = 0; i < prequisites_->length(); ++i)
		prequisites_->at(i)->clean(toolChain);

	PathScope pathScope(projectPath_);

	toolChain->clean(this, modules_, options_);
}

void BuildPlan::distClean(Ref<ToolChain> toolChain)
{
	for (int i = 0; i < prequisites_->length(); ++i)
		prequisites_->at(i)->distClean(toolChain);

	PathScope pathScope(projectPath_);

	toolChain->clean(this, modules_, options_);
	toolChain->distClean(this, modules_, name_, version_, options_);
	unlink("DependencyCache");
}

int BuildPlan::optionsFromRecipe(Ref<Config> recipe)
{
	int options = 0;

	if (recipe->className() == "Library") options |= Library;

	if (recipe->flag("debug")) options |= Debug;
	if (recipe->flag("release")) options |= Release;
	if (recipe->flag("static")) options |= Static;
	if (recipe->flag("optimize-size")) options |= OptimizeSize;
	if (recipe->flag("optimize-speed")) options |= OptimizeSpeed;
	if (recipe->flag("dry-run")) options |= DryRun;
	if (recipe->flag("blindfold")) options |= Blindfold;
	if (recipe->flag("verbose")) options |= Verbose;

	return options;
}

} // namespace ftl
