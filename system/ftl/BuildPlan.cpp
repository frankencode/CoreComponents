#include "PrintDebug.hpp"
#include "Process.hpp"
#include "Glob.hpp"
#include "Config.hpp"
#include "DependencyCache.hpp"
#include "ToolChain.hpp"
#include "BuildPlan.hpp"

namespace ftl
{

class CwdGuard {
public:
	CwdGuard(String path)
		: cwd_(path),
		  cwdSaved_(Process::cwd()),
		  redundant_((cwdSaved_ == cwd_->makeAbsolutePath()) || (cwd_ == "."))
	{
		if (redundant_) return;
		print("Entering %%\n", cwd_);
		Process::cd(cwd_);
	}
	~CwdGuard() {
		if (redundant_) return;
		print("Leaving %%\n", cwd_);
		Process::cd(cwdSaved_);
	}
private:
	String cwd_;
	String cwdSaved_;
	bool redundant_;
};

Ref<BuildPlan, Owner> BuildPlan::newInstance(Ref<ToolChain> toolChain, String projectPath, int globalOptions)
{
	return new BuildPlan(toolChain, projectPath, globalOptions);
}

BuildPlan::BuildPlan(Ref<ToolChain> toolChain, String projectPath, int globalOptions)
	: toolChain_(toolChain),
	  projectPath_(projectPath)
{
	recipe_ = Config::newInstance();
	recipe_->read(projectPath + "/Recipe");

	options_ = optionsFromRecipe(recipe_);

	if (globalOptions != -1) {
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

int BuildPlan::run(int argc, char **argv)
{
	recipe_->read(argc, argv);

	if (recipe_->flag("h") || recipe_->flag("help")) {
		print("no help, yet...\n");
		return 0;
	}

	options_ = optionsFromRecipe(recipe_);
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

	CwdGuard cwdGuard(projectPath_);

	prequisites_ = BuildPlanList::newInstance();

	Ref<StringList, Owner> prequisitePaths;
	if (recipe_->contains("use"))
		prequisitePaths = Ref<VariantList>(recipe_->value("use"))->toList<String>();
	else
		prequisitePaths = StringList::newInstance();

	for (int i = 0; i < prequisitePaths->length(); ++i) {
		String path = prequisitePaths->at(i);
		CwdGuard cwd(path);
		Ref<Config, Owner> recipe = Config::newInstance();
		recipe->read("Recipe");
		if (recipe->className() == "Library") {
			String parentPath = path->reducePath();
			if (!includePaths_->contains(parentPath))
				includePaths_->append(parentPath);
			libraryPaths_->append(path);
			libraries_->append(recipe->value("name"));
		}
		Ref<BuildPlan, Owner> prequisite = BuildPlan::newInstance(toolChain_, path, options_ & GlobalOptions);
		prequisite->analyse();
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
	Ref<DependencyCache, Owner> dependencyCache = DependencyCache::newInstance(toolChain_, sources_, options_, includePaths_);
	for (int i = 0; i < sources_->length(); ++i)
		modules_->set(i, dependencyCache->analyse(sources_->at(i), options_, includePaths_));
}

bool BuildPlan::build()
{
	CwdGuard cwdGuard(projectPath_);

	for (int i = 0; i < prequisites_->length(); ++i)
		if (!prequisites_->at(i)->build()) return false;

	for (int i = 0; i < modules_->length(); ++i) {
		Ref<Module> module = modules_->at(i);
		if (module->dirty()) {
			if (!toolChain_->compile(module, options_, includePaths_)) return false;
		}
	}

	Ref<FileStatus, Owner> targetStatus = FileStatus::newInstance(toolChain_->linkPath(name_, version_, options_));
	if (targetStatus->exists()) {
		Time targetTime = targetStatus->lastModified();
		bool targetDirty = false;
		for (int i = 0; i < modules_->length(); ++i) {
			Ref<Module> module = modules_->at(i);
			if (FileStatus::newInstance(module->modulePath())->lastModified() > targetTime) {
				targetDirty = true;
				break;
			}
		}
		Ref<FileStatus, Owner> recipeStatus = FileStatus::newInstance(recipe_->path());
		if (recipeStatus->exists()) {
			if (recipeStatus->lastModified() > targetTime) targetDirty = true;
		}
		if (!targetDirty) return true;
	}

	return toolChain_->link(modules_, libraryPaths_, libraries_, name_, version_, options_);
}

void BuildPlan::clean()
{
	for (int i = 0; i < prequisites_->length(); ++i)
		prequisites_->at(i)->clean();

	CwdGuard cwdGuard(projectPath_);

	toolChain_->clean(modules_, options_);
}

void BuildPlan::distClean()
{
	for (int i = 0; i < prequisites_->length(); ++i)
		prequisites_->at(i)->distClean();

	CwdGuard cwdGuard(projectPath_);

	toolChain_->clean(modules_, options_);
	toolChain_->distClean(modules_, name_, version_, options_);
	toolChain_->unlink("DependencyCache");
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

	return options;
}

} // namespace ftl
