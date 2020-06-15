/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include "Registry.h"
#include <cc/toki/Registry>
#include <cc/Singleton>

namespace ccclaim {

Registry::Registry()
{}

const HeaderStyle *Registry::headerStyleByLanguage(const String &language) const
{
    return headerStyleByLanguage_->value(language);
}

bool Registry::detectHeaderStyle(const String &path, const String &text, HeaderStyle **style) const
{
    toki::Language *language = nullptr;
    String actualPath = path;
    if (actualPath->fileSuffix() == "qml") actualPath = actualPath->replace(".qml", ".cpp"); // workaround HACK
    if (toki::Registry::instance()->detectLanguage(actualPath, text, &language)) {
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
