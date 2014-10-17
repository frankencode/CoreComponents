/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/Singleton>
#include <flux/toki/Registry>
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
