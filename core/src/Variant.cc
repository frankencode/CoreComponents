/*
 * Copyright (C) 2007-2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/str>
#include <cc/Variant>

namespace cc {

const char *Variant::typeName(VariantType type, VariantType itemType)
{
    switch (type) {
        case VariantType::Undefined: return "undefined";
        case VariantType::Int      : return "integer";
        case VariantType::Bool     : return "boolean";
        case VariantType::Float    : return "float";
        case VariantType::Color    : return "color";
        case VariantType::Version  : return "version";
        case VariantType::Object   : return "object";
        case VariantType::String   : return "string";
        case VariantType::Any      : return "any";
        case VariantType::List     : {
            switch (itemType) {
                case VariantType::Undefined: return "list of undefined";
                case VariantType::Int      : return "list of integer";
                case VariantType::Bool     : return "list of boolean";
                case VariantType::Float    : return "list of float";
                case VariantType::Color    : return "list of color";
                case VariantType::Version  : return "list of version";
                case VariantType::Object   : return "list of object";
                case VariantType::String   : return "list of string";
                case VariantType::Any      : return "list of any";
                case VariantType::List     : return "list of list";
                case VariantType::Map      : return "list of map";
            }
            return "list of unkown";
        }
        case VariantType::Map     : {
            switch (itemType) {
                case VariantType::Undefined: return "map of undefined";
                case VariantType::Int      : return "map of integer";
                case VariantType::Bool     : return "map of boolean";
                case VariantType::Float    : return "map of float";
                case VariantType::Color    : return "map of color";
                case VariantType::Version  : return "map of version";
                case VariantType::Object   : return "map of object";
                case VariantType::String   : return "map of string";
                case VariantType::Any      : return "map of any";
                case VariantType::List     : return "map of list";
                case VariantType::Map      : return "map of map";
            }
            return "map of unkown";
        }
    }
    return "unknown";
}

Variant Variant::read(String s)
{
    if (s->beginsWith('"') && s->endsWith('"'))
        return Variant(s->copy(1, s->count() - 1));

    if (s->contains('.') || s->contains('e') || s->contains('E')) {
        double value = 0;
        if (s->scanNumber(&value) == s->count()) return Variant(value);
    }

    int value = 0;
    if (s->scanNumber(&value) == s->count()) return Variant(value);
    if (s == "true" || s == "on") return Variant(true);
    if (s == "false" || s == "off") return Variant(false);

    return Variant(s);
}

const char *Variant::Instance::typeName() const
{
    return Variant::typeName(type_, itemType_);
}

String Variant::Instance::toString() const
{
    switch (type_) {
        case VariantType::Undefined: return String{};
        case VariantType::Bool     : return str(int_ != 0);
        case VariantType::Int      : return str(int_);
        case VariantType::Float    : return str(float_);
        case VariantType::Color    : return str(Color::fromWord(word_));
        case VariantType::Version  : return str(Version::fromWord(word_));
        case VariantType::String   : return Variant::cast<CharArray *>(ref().get());
        case VariantType::Object   : return str((void *)Variant::cast<Object *>(ref().get()));
        default                    : return String{};
    };

    return String{};
}

bool Variant::operator==(const Variant &b) const
{
    bool equal = false;

    if (+((*this)->type_ & VariantType::Int) && +(b->type_ & VariantType::Int))
        equal = ((*this)->int_ == b->int_);
    else if (+((*this)->type_ & VariantType::Float) && +(b->type_ & VariantType::Float))
        equal = ((*this)->float_ == b->float_);
    else if (+((*this)->type_ & VariantType::Color) && +(b->type_ & VariantType::Color))
        equal = ((*this)->word_ == b->word_);
    else if (+((*this)->type_ & VariantType::Version) && +(b->type_ & VariantType::Version))
        equal = ((*this)->word_ == b->word_);
    else if (((*this)->type_ == VariantType::String) && (b->type_ == VariantType::String))
        equal = Variant::cast<CharArray *>(*this)->equals(Variant::cast<CharArray *>(b));
    else if (((*this)->type_ == VariantType::Object) && (b->type_ == VariantType::Object))
        equal = ((*this)->ref().get() == b->ref().get());
    else
        equal = (((*this)->type_ == VariantType::Undefined) && (b->type_ == VariantType::Undefined));

    return equal;
}

bool Variant::operator<(const Variant &b) const
{
    bool below = false;

    if (+((*this)->type_ & VariantType::Int) && +(b->type_ & VariantType::Int))
        below = ((*this)->int_ < b->int_);
    else if (+((*this)->type_ & VariantType::Float) && +(b->type_ & VariantType::Float))
        below = ((*this)->float_ < b->float_);
    else if (+((*this)->type_ & VariantType::Version) && +(b->type_ & VariantType::Version))
        below = (Version::fromWord((*this)->word_) < Version::fromWord(b->word_));
    else if (((*this)->type_ == VariantType::String) && (b->type_ == VariantType::String))
        below = Variant::cast<CharArray *>(*this)->below(Variant::cast<CharArray *>(b));
    else if (((*this)->type_ == VariantType::Object) && (b->type_ == VariantType::Object))
        below = ((*this)->ref().get() < b->ref().get());

    return below;
}

} // namespace cc
