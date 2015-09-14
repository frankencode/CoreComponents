/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/exceptions>
#include <flux/Format>
#include <flux/Arguments>

namespace flux {

Ref<Arguments> Arguments::parse(int argc, char **argv)
{
    return new Arguments(argc, argv);
}

Arguments::Arguments(int argc, char **argv):
    options_(VariantMap::create()),
    items_(StringList::create())
{
    execPath_ = argv[0];

    for (int i = 1; i < argc; ++i)
    {
        String s = argv[i];
        if (s->at(0) != '-' && !s->contains('=')) {
            items_->append(s);
            continue;
        }

        Ref<StringList> parts = s->trimInsitu("-")->split("=");
        String name = parts->pop(0);
        String valueText = parts->join("=");
        Variant value = true;
        if (valueText != "") value = Variant::read(valueText);

        options_->establish(name, value);
    }

    if (options_->contains("h") || options_->contains("help") || options_->contains("?"))
        throw HelpError();

    // FIXME
    // if (options_->contains("v") || options_->contains("version"))
    //  throw VersionError();
}

void Arguments::validate(VariantMap *prototype) const
{
    for (int i = 0; i < options_->count(); ++i)
    {
        String name = options_->keyAt(i);
        Variant value = options_->valueAt(i);

        Variant defaultValue;
        if (!prototype->lookup(name, &defaultValue))
            throw UsageError(Format("No such option: \"%%\"") << name);
        if (defaultValue == Variant()) continue;
        int valueType = type(value);
        int defaultType = type(defaultValue);
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
                    Format("Option \"%%\" expects type %%") << name << Variant::typeName(type(defaultValue), itemType(defaultValue))
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

} // namespace flux
