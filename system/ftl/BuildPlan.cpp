#include "PrintDebug.hpp"
#include "File.hpp"
#include "Process.hpp"
#include "Glob.hpp"
#include "Config.hpp"
#include "DependencyCache.hpp"
#include "ToolChain.hpp"
#include "GccToolChain.hpp"
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

Ref<BuildPlan, Owner> BuildPlan::create(int argc, char **argv)
{
	return new BuildPlan(argc, argv);
}

Ref<BuildPlan, Owner> BuildPlan::create(Ref<ToolChain> toolChain, String projectPath, int globalOptions)
{
	return new BuildPlan(toolChain, projectPath, globalOptions);
}

BuildPlan::BuildPlan(int argc, char **argv)
	: toolChain_(GccToolChain::create()),
	  projectPath_(".")
{
	recipe_ = Config::create();
	recipe_->read(argc, argv);
	if (recipe_->arguments()->length() > 0) {
		if (recipe_->arguments()->length() > 1)
			FTL_THROW(BuildPlanException, "Processing multiple Recipe files at once is not supported");
		projectPath_ = recipe_->arguments()->at(0);
	}
	recipe_->clear();
	recipe_->read(projectPath_ + "/Recipe");
	recipe_->read(argc, argv);
	readRecipe();
}

BuildPlan::BuildPlan(Ref<ToolChain> toolChain, String projectPath, int globalOptions)
	: toolChain_(toolChain),
	  projectPath_(projectPath)
{
	recipe_ = Config::create();
	recipe_->read(projectPath_ + "/Recipe");
	readRecipe();

	if (globalOptions != Unspecified) {
		options_ &= ~GlobalOptions;
		options_ |= globalOptions;
	}
}

void BuildPlan::readRecipe()
{
	options_ = 0;

	if (recipe_->className() == "Library") options_ |= Library;

	if (recipe_->flag("debug"))          options_ |= Debug;
	if (recipe_->flag("release"))        options_ |= Release;
	if (recipe_->flag("static"))         options_ |= Static;
	if (recipe_->flag("optimize-size"))  options_ |= OptimizeSize;
	if (recipe_->flag("optimize-speed")) options_ |= OptimizeSpeed;
	if (recipe_->flag("dry-run"))        options_ |= DryRun;
	if (recipe_->flag("blindfold"))      options_ |= Blindfold;
	if (recipe_->flag("verbose"))        options_ |= Verbose;

	name_ = recipe_->value("name");
	version_ = recipe_->value("version");

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
}

String BuildPlan::sourcePath(String source) const
{
	if (projectPath_ == ".") return source;
	return projectPath_ + "/" + source;
}

String BuildPlan::runAnalyse(String command)
{
	if (options_ & Verbose) error()->writeLine(command);
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
	if (File::unresolvedStatus(path)->exists()) {
		printTo(error(), "rm %%\n", path);
		if (options_ & DryRun) return true;
		return File::unlink(path);
	}
	return true;
}

Ref<FileStatus, Owner> BuildPlan::fileStatus(String path)
{
	if (options_ & Blindfold) return FileStatus::create();
	return FileStatus::create(path);
}

int BuildPlan::run()
{
	if (recipe_->flag("h") || recipe_->flag("help")) {
		print("no help, yet...\n");
		return 0;
	}

	analyse();

	if (recipe_->flag("c") || recipe_->flag("clean")) {
		clean();
		return 0;
	}
	if (recipe_->flag("dist-clean")) {
		distClean();
		return 0;
	}

	return build() ? 0 : 1;
}

void BuildPlan::analyse()
{
	if (sources_) return;

	bool verbose = options_ & Verbose;
	PathScope pathScope(projectPath_, verbose);

	prequisites_ = BuildPlanList::create();

	Ref<StringList, Owner> prequisitePaths;
	if (recipe_->contains("use"))
		prequisitePaths = Ref<VariantList>(recipe_->value("use"))->toList<String>();
	else
		prequisitePaths = StringList::create();

	for (int i = 0; i < prequisitePaths->length(); ++i) {
		String path = prequisitePaths->at(i);
		PathScope cwd(path, verbose);
		Ref<Config, Owner> recipe = Config::create();
		recipe->read("Recipe");
		if (recipe->className() == "Library") {
			String parentPath = path->reducePath();
			if (!includePaths_->contains(parentPath))
				includePaths_->append(parentPath);
			libraryPaths_->append(path);
			libraries_->append(recipe->value("name"));
		}
		Ref<BuildPlan, Owner> buildPlan = BuildPlan::create(toolChain_, path, options_ & GlobalOptions);
		buildPlan->analyse();
		prequisites_->append(buildPlan);
	}

	sources_ = StringList::create();
	Ref<VariantList> sourcePatterns = recipe_->value("source");
	Ref<DirEntry, Owner> entry = DirEntry::create();
	for (int i = 0; i < sourcePatterns->length(); ++i) {
		Ref<Glob, Owner> glob = Glob::open(sourcePatterns->at(i));
		for (String path; glob->read(&path);)
			sources_->append(path);
	}

	modules_ = ModuleList::create(sources_->length());
	Ref<DependencyCache, Owner> dependencyCache = DependencyCache::create(this);
	for (int i = 0; i < sources_->length(); ++i)
		modules_->set(i, dependencyCache->analyse(sources_->at(i)));
}

bool BuildPlan::build()
{
	PathScope pathScope(projectPath_);

	for (int i = 0; i < prequisites_->length(); ++i)
		if (!prequisites_->at(i)->build()) return false;

	for (int i = 0; i < modules_->length(); ++i) {
		Ref<Module> module = modules_->at(i);
		if (module->dirty()) {
			if (!toolChain_->compile(this, module)) return false;
		}
	}

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
		if (!targetDirty) return true;
	}

	return toolChain_->link(this);
}

void BuildPlan::clean()
{
	for (int i = 0; i < prequisites_->length(); ++i)
		prequisites_->at(i)->clean();

	PathScope pathScope(projectPath_);

	toolChain_->clean(this);
}

void BuildPlan::distClean()
{
	for (int i = 0; i < prequisites_->length(); ++i)
		prequisites_->at(i)->distClean();

	PathScope pathScope(projectPath_);

	toolChain_->distClean(this);
	unlink("DependencyCache");
}

} // namespace ftl
