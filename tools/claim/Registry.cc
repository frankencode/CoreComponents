/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Singleton>
#include <cc/toki/Registry>
#include "Registry.h"

namespace ccclaim {

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
    if (path->fileSuffix() == "qml") path = path->replace(".qml", ".cpp"); // workaround HACK
    if (toki::Registry::instance()->detectLanguage(path, text, &language)) {
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

} // namespace ccclaim
