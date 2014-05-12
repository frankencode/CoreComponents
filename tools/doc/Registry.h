#ifndef FLUXDOC_REGISTRY_H
#define FLUXDOC_REGISTRY_H

#include <flux/Singleton.h>
#include <flux/yason.h>

namespace fluxdoc
{

using namespace flux;

class Generator;

class Registry: public Object, public Singleton<Registry>
{
public:
	Generator* generatorByName(String name) const;
	Generator* generatorByIndex(int index) const;
	int generatorCount() const;

	YasonProtocol *recipeProtocol() const;

private:
	friend class Singleton<Registry>;
	friend class Generator;

	Registry();
	void registerGenerator(Generator *generator);

	typedef Map< String, Ref<Generator> > GeneratorByName;
	Ref<GeneratorByName> generatorByName_;

	Ref<YasonProtocol> recipeProtocol_;
};

inline Registry *registry() { return Registry::instance(); }

} // namespace fluxdoc

#endif // FLUXDOC_REGISTRY_H
