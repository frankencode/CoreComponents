/*
 * Copyright (C) 2013-2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXCLAIM_REGISTRY_H
#define FLUXCLAIM_REGISTRY_H

#include <flux/Map.h>
#include "HeaderStyle.h"

namespace flux { template<class> class Singleton; }

namespace fluxclaim {

using namespace flux;

class Registry: public Object
{
public:
	inline int headerStyleCount() const { return headerStyleByLanguage_->count(); }
	HeaderStyle *headerStyleAt(int i) const { return headerStyleByLanguage_->valueAt(i); }

	const HeaderStyle *headerStyleByLanguage(String language) const;
	bool detectHeaderStyle(String path, String text, HeaderStyle **style) const;

private:
	friend class Singleton<Registry>;
	friend class HeaderStyle;

	Registry();

	void registerHeaderStyle(HeaderStyle *style);

	typedef Map< String, Ref<HeaderStyle> > HeaderStyleByLanguage;
	Ref<HeaderStyleByLanguage> headerStyleByLanguage_;
};

Registry *registry();

} // namespace fluxclaim

#endif // FLUXCLAIM_REGISTRY_H
