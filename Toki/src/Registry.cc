/*
 * Copyright (C) 2007-2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/toki/Registry>
#include <cc/Map>

namespace cc::toki {

struct Registry::State final: public Singleton::State
{
    bool detectLanguage(const String &path, const String &text, Out<Language> language) const
    {
        List<Language> candidates;

        for (const auto &pair: languageByName_) {
            const Language &candidate = pair.value();
            if (candidate.pathPattern().match(path)) {
                language = candidate;
                return true;
            }
        }

        for (const auto &pair: languageByName_) {
            const Language &candidate = pair.value();
            if (candidate.discoverySyntax()) {
                if (candidate.discoverySyntax().match(text)) {
                    language = candidate;
                    return true;
                }
            }
        }

        return false;
    }

    Map<String, Language> languageByName_;
};

Registry::Registry():
    Singleton{instance<State>()}
{}

bool Registry::lookupLanguageByName(const String &name, Out<Language> language) const
{
    return me().languageByName_.lookup(name, language);
}

bool Registry::detectLanguage(const String &path, const String &text, Out<Language> language) const
{
    return me().detectLanguage(path, text, language);
}

int Registry::languageCount() const
{
    return me().languageByName_.count();
}

const Language &Registry::languageAt(int i) const
{
    return me().languageByName_.at(i).value();
}

void Registry::registerLanguage(const Language &language)
{
    me().languageByName_.insert(language.name(), language);
}

Registry::State &Registry::me()
{
    return Object::me.as<State>();
}

const Registry::State &Registry::me() const
{
    return Object::me.as<State>();
}

} // namespace cc::toki
