/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Format>
#include <cc/Arguments>

namespace cc {

Ref<Arguments> Arguments::parse(int argc, char **argv, VariantMap *options)
{
    return new Arguments(argc, argv, options);
}

Arguments::Arguments(int argc, char **argv, VariantMap *options):
    options_(VariantMap::create()),
    items_(StringList::create())
{
    execPath_ = argv[0];

    for (int i = 1; i < argc; ++i)
    {
        String s = argv[i];
        bool isKeyValueOption = s->contains('=');
        if (!s->beginsWith('-')) {
            if (!s->contains('=')) {
                items_->append(s);
                continue;
            }
        }

        if (s->beginsWith('-')) s->trimInsitu("-");

        Variant value = true;
        if (isKeyValueOption) {
            Ref<StringList> parts = s->split('=');
            String name = parts->pop(0);
            String valueText = parts->join("=");
            options_->establish(name, Variant::read(valueText));
        }
        else
            options_->establish(s, true);
    }

    if (options_->contains("h") || options_->contains("help") || options_->contains("?"))
        throw HelpRequest();

    if (options_->contains("v") || options_->contains("version"))
        throw VersionRequest();

    if (options) {
        validate(options);
        override(options);
    }
}

void Arguments::validate(const VariantMap *prototype)
{
    for (int i = 0; i < options_->count(); ++i)
    {
        String name = options_->keyAt(i);
        Variant value = options_->valueAt(i);

        Variant defaultValue;
        if (!prototype->lookup(name, &defaultValue))
            throw UsageError(Format("No such option: \"%%\"") << name);
        if (defaultValue == Variant()) continue;
        int valueType = Variant::type(value);
        int defaultType = Variant::type(defaultValue);
        if (valueType != defaultType) {
            if (valueType == Variant::IntType && defaultType == Variant::BoolType) {
                int intValue = value;
                if (intValue == 0 || intValue == 1)
                    options_->establish(name, intValue == 1);
            }
            else if (valueType == Variant::IntType && defaultType == Variant::FloatType) {
                options_->establish(name, float(int(value)));
            }
            else if (valueType != Variant::ObjectType && defaultType == Variant::StringType) {
                options_->establish(name, str(value));
            }
            else {
                throw UsageError(
                    Format("Option \"%%\" expects type %% (got %%: %%)")
                        << name
                        << Variant::typeName(Variant::type(defaultValue), Variant::itemType(defaultValue))
                        << Variant::typeName(Variant::type(value), Variant::itemType(value))
                        << value
                );
            }
        }
    }
}

void Arguments::override(VariantMap *config) const
{
    for (int i = 0; i < options_->count(); ++i)
    {
        String name = options_->keyAt(i);
        Variant value = options_->valueAt(i);
        config->establish(name, value);
    }
}

} // namespace cc
