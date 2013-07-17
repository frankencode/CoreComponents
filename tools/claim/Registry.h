#ifndef FCLAIM_REGISTRY_H
#define FCLAIM_REGISTRY_H

#include <fkit/Singleton.h>
#include <fkit/List.h>
#include "HeaderScanner.h"

namespace fclaim
{

using namespace fkit;

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

#endif // FCLAIM_REGISTRY_H
