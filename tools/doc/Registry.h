#ifndef FLUXDOC_REGISTRY_H
#define FLUXDOC_REGISTRY_H

#include <flux/Singleton.h>
#include <flux/Map.h>

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

private:
	friend class Singleton<Registry>;
	friend class Generator;

	Registry();
	void registerGenerator(Generator *generator);

	typedef Map< String, Ref<Generator> > GeneratorByName;
	Ref<GeneratorByName> generatorByName_;
};

inline Registry *registry() { return Registry::instance(); }

} // namespace fluxdoc

#endif // FLUXDOC_REGISTRY_H
