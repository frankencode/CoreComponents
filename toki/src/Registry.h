/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXTOKI_REGISTRY_H
#define FLUXTOKI_REGISTRY_H

#include <flux/SyntaxDefinition>
#include <flux/Registration>
#include <flux/Map>
#include <flux/toki/Language>
#include <flux/toki/Theme>

namespace flux {

template<class> class Singleton;

namespace toki {

class Registry: public Object
{
public:
    bool lookupLanguageByName(String name, Language **language) const;
    bool detectLanguage(String path, String text, Language **language) const;

    inline int languageCount() const { return languageByName_->count(); }
    inline Language *languageAt(int i) const { return languageByName_->valueAt(i); }

private:
    friend class Singleton<Registry>;
    friend class Language;

    Registry();

    void registerLanguage(Language *language);

    typedef Map< String, Ref<Language> > LanguageByName;
    Ref<LanguageByName> languageByName_;
};

Registry *registry();

}} // namespace flux::toki

#endif // FLUXTOKI_REGISTRY_H
