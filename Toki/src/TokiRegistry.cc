/*
 * Copyright (C) 2007-2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/TokiRegistry>
#include <cc/Map>

namespace cc {

struct TokiRegistry::State final: public Singleton::State
{
    bool detectLanguage(const String &path, const String &text, Out<TokiLanguage> language) const
    {
        List<TokiLanguage> candidates;

        for (const auto &pair: languageByName_) {
            const TokiLanguage &candidate = pair.value();
            if (candidate.pathPattern().match(path)) {
                language = candidate;
                return true;
            }
        }

        for (const auto &pair: languageByName_) {
            const TokiLanguage &candidate = pair.value();
            if (candidate.discoverySyntax()) {
                if (candidate.discoverySyntax().match(text)) {
                    language = candidate;
                    return true;
                }
            }
        }

        return false;
    }

    Map<String, TokiLanguage> languageByName_;
};

TokiRegistry::TokiRegistry():
    Singleton{instance<State>()}
{}

bool TokiRegistry::lookupLanguageByName(const String &name, Out<TokiLanguage> language) const
{
    return me().languageByName_.lookup(name, language);
}

bool TokiRegistry::detectLanguage(const String &path, const String &text, Out<TokiLanguage> language) const
{
    return me().detectLanguage(path, text, language);
}

int TokiRegistry::languageCount() const
{
    return me().languageByName_.count();
}

const TokiLanguage &TokiRegistry::languageAt(int i) const
{
    return me().languageByName_.at(i).value();
}

void TokiRegistry::registerLanguage(const TokiLanguage &language)
{
    me().languageByName_.insert(language.name(), language);
}

TokiRegistry::State &TokiRegistry::me()
{
    return Object::me.as<State>();
}

const TokiRegistry::State &TokiRegistry::me() const
{
    return Object::me.as<State>();
}

} // namespace cc
