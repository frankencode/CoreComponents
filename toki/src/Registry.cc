/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Singleton>
#include <cc/toki/Registry>

namespace cc {
namespace toki {

Registry *Registry::instance()
{
    return Singleton<Registry>::instance();
}

Registry::Registry():
    languageByName_{LanguageByName::create()}
{}

bool Registry::lookupLanguageByName(String name, Language **language) const
{
    return languageByName_->lookup(name, language);
}

bool Registry::detectLanguage(String path, String text, Language **language) const
{
    typedef List<Language *> Candidates;
    Ref<Candidates> candidates = Candidates::create();

    for (int i = 0; i < languageByName_->count(); ++i) {
        Language *candidate = languageByName_->valueAt(i);
        if (candidate->pathPattern()->match(path)->valid())
            candidates->append(candidate);
    }

    if (candidates->count() == 1) {
        *language = candidates->at(0);
        return true;
    }

    for (int i = 0; i < candidates->count(); ++i) {
        Language *candidate = candidates->at(i);
        if (candidate->discoverySyntax()) {
            if (candidate->discoverySyntax()->match(text)->valid()) {
                *language = candidate;
                return true;
            }
        }
    }

    return false;
}

void Registry::registerLanguage(Language *language)
{
    languageByName_->insert(language->name(), language);
}

}} // namespace cc::toki
