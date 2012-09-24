#include "stdio"
#include "Config.hpp"
#include "DependencyCache.hpp"
#include "ToolChain.hpp"

namespace ftl
{

ToolChain::ToolChain()
{}

void ToolChain::init(String execPath, String machine)
{
	execPath_ = execPath;
	machine_ = machine;
}

int ToolChain::optionsFromRecipe(Ref<Config> recipe) const
{
	int options = 0;

	if (recipe->className() == "Library") options |= Library;

	if (recipe->flag("debug")) options |= Debug;
	if (recipe->contains("release")) options |= Release;
	if (recipe->contains("static")) options |= Static;
	if (recipe->contains("optimize-size")) options |= OptimizeSize;
	if (recipe->contains("optimize-speed")) options |= OptimizeSpeed;

	return options;
}

Ref<ModuleList, Owner> ToolChain::analyse(Ref<Config> recipe)
{
	Ref<StringList, Owner> sources = StringList::newInstance();
	{
		Ref<VariantList> sourcePatterns = recipe->value("source");
		Ref<DirEntry, Owner> entry = DirEntry::newInstance();
		for (int i = 0; i < sourcePatterns->length(); ++i) {
			Ref<Glob, Owner> glob = Glob::newInstance(sourcePatterns->at(i));
			while (glob->read(entry))
				sources->append(entry->path());
		}
	}

	int options = optionsFromRecipe(recipe);
	Ref<ModuleList, Owner> modules = ModuleList::newInstance(sources->length());

	Ref<DependencyCache, Owner> dependencyCache = DependencyCache::newInstance(this, sources, options);
	for (int i = 0; i < sources->length(); ++i)
		modules->set(i, dependencyCache->analyse(sources->at(i), options));

	return modules;
}

bool ToolChain::build(Ref<Config> recipe)
{
	int options = optionsFromRecipe(recipe);
	Ref<ModuleList, Owner> modules = analyse(recipe);

	for (int i = 0; i < modules->length(); ++i) {
		Ref<Module> module = modules->at(i);
		if (module->dirty()) {
			if (!compile(module, options)) return false;
		}
	}

	String name = recipe->value("name");
	String version = recipe->value("version");

	Ref<FileStatus, Owner> targetStatus = FileStatus::newInstance(linkPath(name, version, options));
	if (targetStatus->exists()) {
		Time targetTime = targetStatus->lastModified();
		bool targetDirty = false;
		for (int i = 0; i < modules->length(); ++i) {
			Ref<Module> module = modules->at(i);
			if (FileStatus::newInstance(module->modulePath())->lastModified() > targetTime) {
				targetDirty = true;
				break;
			}
		}
		if (!targetDirty) return true;
	}

	return link(modules, Ref<VariantList>(recipe->value("link"))->toList<String>(), name, version, options);
}

void ToolChain::clean(Ref<Config> recipe)
{
	Ref<ModuleList, Owner> modules = analyse(recipe);
	try {
		for (int i = 0; i < modules->length(); ++i) {
			Ref<Module> module = modules->at(i);
			File::newInstance(module->modulePath())->unlink();
		}
		File::newInstance(
			linkPath(
				recipe->value("name"),
				recipe->value("version"),
				optionsFromRecipe(recipe)
			)
		)->unlink();
	}
	catch (SystemException &)
	{}
}

} // namespace ftl
