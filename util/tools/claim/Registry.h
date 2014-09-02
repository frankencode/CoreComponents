/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
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
