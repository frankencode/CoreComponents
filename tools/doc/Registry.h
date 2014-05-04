#ifndef FLUXDOC_REGISTRY_H
#define FLUXDOC_REGISTRY_H

#include <flux/Singleton.h>
#include <flux/Map.h>
#include "OutputFormat.h"

namespace fluxdoc
{

using namespace flux;

class Registry: public Object, public Singleton<Registry>
{
public:
	void registerOutputFormat(OutputFormat *format);

	OutputFormat* outputFormatByName(String name) const;
	OutputFormat* outputFormatByIndex(int index) const;
	int outputFormatCount() const;

private:
	friend class Singleton<Registry>;
	Registry();
	typedef Map< String, Ref<OutputFormat> > OutputFormatByName;
	Ref<OutputFormatByName> outputFormatByName_;
};

inline Registry *registry() { return Registry::instance(); }

} // namespace fluxdoc

#endif // FLUXDOC_REGISTRY_H
