/*
 * Copyright (C) 2007-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Arguments>
#include <cc/Format>

namespace cc {

Arguments::Instance::Instance(int argc, char *argv[])
{
    execPath_ = argv[0];

    for (int i = 1; i < argc; ++i)
    {
        String s = argv[i];
        bool isKeyValueOption = s->contains('=');
        bool isFlag = s->startsWith('-') && s->count() >= 2 && (s->at(1) < '0' || '9' < s->at(1)) && s->at(1) != '.';
        if (!isFlag && !isKeyValueOption) {
            items_->append(s);
            continue;
        }

        if (isFlag) mutate(s)->trimInsitu("-");

        Variant value = true;
        if (isKeyValueOption) {
            StringList parts = s->split('=');
            String name = parts->front();
            parts->popFront();
            String valueText = parts->join("=");
            options_->establish(name, Variant::read(valueText));
        }
        else {
            bool value = true;
            if (s->startsWith("no-")) {
                value = false;
                s = s->copy(3, s->count());
            }
            options_->establish(s, value);
        }
    }

    if (options_->contains("h") || options_->contains("help") || options_->contains("?"))
        throw HelpRequest{};

    if (options_->contains("v") || options_->contains("version"))
        throw VersionRequest{};
}

void Arguments::Instance::validate(const VariantMap &prototype)
{
    for (int i = 0; i < options_->count(); ++i)
    {
        String name = options_->at(i)->key();
        Variant value = options_->at(i)->value();

        Variant defaultValue;
        if (!prototype->lookup(name, &defaultValue))
            throw UsageError{Format{"No such option: \"%%\""} << name};
        if (defaultValue == Variant{}) continue;
        if (value->type() != defaultValue->type()) {
            if (value->type() == VariantType::Int && defaultValue->type() == VariantType::Bool) {
                int intValue = value;
                if (intValue == 0 || intValue == 1)
                    options_->establish(name, intValue == 1);
            }
            else if (value->type() == VariantType::Int && defaultValue->type() == VariantType::Float) {
                options_->establish(name, float(int(value)));
            }
            else if (value->type() != VariantType::Object && defaultValue->type() == VariantType::String) {
                options_->establish(name, str(value));
            }
            else if (value->type() == VariantType::String && defaultValue->type() == VariantType::List && defaultValue->itemType() == VariantType::String) {
                options_->establish(name, StringList{str(value)});
            }
            else if (value->type() == VariantType::Int && defaultValue->type() == VariantType::List && defaultValue->itemType() == VariantType::Int) {
                options_->establish(name, List<int>{int(value)});
            }
            else {
                throw UsageError{
                    Format{"Option \"%%\" expects type %% (got %%: %%)"}
                        << name
                        << defaultValue->typeName()
                        << value->typeName()
                        << value
                };
            }
        }
    }
}

void Arguments::Instance::override(VariantMap &config) const
{
    for (int i = 0; i < options_->count(); ++i)
    {
        String name = options_->at(i)->key();
        Variant value = options_->at(i)->value();
        config->establish(name, value);
    }
}

} // namespace cc
