#ifndef FLUXCLAIM_REGISTRY_H
#define FLUXCLAIM_REGISTRY_H

#include <flux/Singleton.h>
#include <flux/List.h>
#include "HeaderScanner.h"

namespace fclaim
{

using namespace flux;

class Registry: public Singleton<Registry>, public Object
{
public:
	const HeaderScannerList *headerScanners() const;
	void registerHeaderScanner(HeaderScanner *scanner);

private:
	friend class Singleton<Registry>;
	Registry();

	Ref<HeaderScannerList> headerScanners_;
};

inline Registry *registry() { return Registry::instance(); }

} // namespace fclaim

#endif // FLUXCLAIM_REGISTRY_H
