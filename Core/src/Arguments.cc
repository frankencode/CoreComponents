/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/Arguments>
#include <cc/Format>

namespace cc {

Arguments::Arguments(int argc, char *argv[]):
    me{argv[0]}
{
    for (int i = 1; i < argc; ++i)
    {
        String s = argv[i];
        bool isKeyValue = s.find('=');
        bool isFlag = s.startsWith('-') && s.count() >= 2 && (s.at(1) < '0' || '9' < s.at(1)) && s.at(1) != '.';
        if (!isFlag && !isKeyValue) {
            me().items.append(s);
            continue;
        }

        if (isFlag) s.trim("-");

        Variant value = true;
        if (isKeyValue) {
            List<String> parts = s.split('=');
            String name = parts.first();
            parts.popFront();
            String valueText{parts, '='};
            me().options.establish(name, Variant::read(valueText));
        }
        else {
            bool value = true;
            if (s.startsWith("no-")) {
                value = false;
                s = s.copy(3, s.count());
            }
            me().options.establish(s, value);
        }
    }

    if (me().options.find("h") || me().options.find("help") || me().options.find("?"))
        throw HelpRequest{};

    if (me().options.find("v") || me().options.find("version"))
        throw VersionRequest{};
}

void Arguments::validate(const Map<String, Variant> &prototype)
{
    for (Locator pos = me().options.head(); pos; ++pos)
    {
        String name = me().options.at(pos).key();
        Variant value = me().options.at(pos).value();

        Variant defaultValue;
        if (!prototype.lookup(name, &defaultValue))
            throw UsageError{Format{"No such option: \"%%\""} << name};

        if (defaultValue.is<void>()) continue;

        if (!value.sameTypeAs(defaultValue)) {
            if (value.is<long>() && defaultValue.is<bool>()) {
                long intValue = long(value);
                if (intValue == 0 || intValue == 1)
                    me().options.at(pos).value() = (intValue == 1);
            }
            else if (value.is<long>() && defaultValue.is<double>()) {
                me().options.at(pos).value() = double(long(value));
            }
            else {
                throw UsageError{
                    Format{"Option \"%%\" expects type %% (got %%: %%)"}
                        << name
                        << defaultValue.typeName()
                        << value.typeName()
                        << value
                };
            }
        }
    }
}

void Arguments::override(InOut<Map<String, Variant>> config) const
{
    for (auto item: me().options)
        config().establish(item.key(), item.value());
}

} // namespace cc
