/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXCLAIM_REGISTRY_H
#define FLUXCLAIM_REGISTRY_H

#include <flux/Singleton.h>
#include <flux/List.h>
#include "HeaderScanner.h"

namespace fluxclaim
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

} // namespace fluxclaim

#endif // FLUXCLAIM_REGISTRY_H
