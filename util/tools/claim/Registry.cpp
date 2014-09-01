/*
 * Copyright (C) 2013-2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <flux/Singleton.h>
#include <flux/toki/Registry.h>
#include "Registry.h"

namespace fluxclaim {

Registry::Registry():
	  headerStyleByLanguage_(HeaderStyleByLanguage::create())
{}

const HeaderStyle *Registry::headerStyleByLanguage(String language) const
{
	return headerStyleByLanguage_->value(language);
}

bool Registry::detectHeaderStyle(String path, String text, HeaderStyle **style) const
{
	toki::Language *language = 0;
	if (toki::registry()->detectLanguage(path, text, &language)) {
		Ref<HeaderStyle> value;
		if (headerStyleByLanguage_->lookup(language->name(), &value)) {
			*style = value;
			return true;
		}
	}
	return false;
}

void Registry::registerHeaderStyle(HeaderStyle *style)
{
	headerStyleByLanguage_->insert(style->language(), style);
}

Registry *registry() { return Singleton<Registry>::instance(); }

} // namespace fluxclaim
