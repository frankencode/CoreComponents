/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Registration>
#include <cc/Map>
#include <cc/syntax/SyntaxDefinition>
#include <cc/toki/Language>
#include <cc/toki/Theme>

namespace cc {

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

}} // namespace cc::toki

