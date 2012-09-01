#include "stdio"
#include "Wire.hpp"
#include "MachDependencyCache.hpp"
#include "MachCompiler.hpp"

namespace ftl
{

MachCompiler::MachCompiler()
{}

void MachCompiler::init(String execPath, String machine)
{
	execPath_ = execPath;
	machine_ = machine;
}

int MachCompiler::optionsFromRecipe(Ref<WireObject> recipe) const
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

Ref<MachObjectList, Owner> MachCompiler::analyse(Ref<WireObject> recipe)
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
	Ref<MachObjectList, Owner> objects = MachObjectList::newInstance(sources->length());

	Ref<MachDependencyCache, Owner> dependencyCache = MachDependencyCache::newInstance(this, sources, options);
	for (int i = 0; i < sources->length(); ++i)
		objects->set(i, dependencyCache->analyse(sources->at(i), options));

	return objects;
}

bool MachCompiler::build(Ref<WireObject> recipe)
{
	int options = optionsFromRecipe(recipe);
	Ref<MachObjectList, Owner> objects = analyse(recipe);

	for (int i = 0; i < objects->length(); ++i) {
		Ref<MachObject> object = objects->at(i);
		if (object->dirty()) {
			if (!compile(object, options)) return false;
		}
	}

	return link(
		objects,
		Ref<VariantList>(recipe->value("link"))->toList<String>(),
		recipe->value("name"),
		recipe->value("version"),
		options
	);
}

void MachCompiler::clean(Ref<WireObject> recipe)
{
	Ref<MachObjectList, Owner> objects = analyse(recipe);
	for (int i = 0; i < objects->length(); ++i) {
		Ref<MachObject> object = objects->at(i);
		File::newInstance(object->objectPath())->unlink();
	}
}

} // namespace ftl
