/*
 * Copyright (C) 2007-2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/claim/Registry>
#include <cc/toki/Registry>

namespace cc::claim {

struct Registry::State final: public Object::State
{
    Map<String, HeaderStyle> headerStyleByLanguage_;
};

Registry::Registry():
    Singleton{instance<State>()}
{}

long Registry::headerStyleCount() const
{
    return me().headerStyleByLanguage_.count();
}

const HeaderStyle &Registry::headerStyleAt(long i) const
{
    return me().headerStyleByLanguage_.at(i).value();
}

bool Registry::detectHeaderStyle(const String &path, const String &text, Out<HeaderStyle> style) const
{
    toki::Language language;
    if (toki::Registry{}.detectLanguage(path, text, &language)) {
        if (me().headerStyleByLanguage_.lookup(language.name(), style)) {
            return true;
        }
    }
    return false;
}

void Registry::registerHeaderStyle(const HeaderStyle &style)
{
    me().headerStyleByLanguage_.establish(style.language(), style);
}

Registry::State &Registry::me()
{
    return Object::me.as<State>();
}

const Registry::State &Registry::me() const
{
    return Object::me.as<State>();
}

} // namespace cc::claim
