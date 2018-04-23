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

const char *Variant::typeName(int type, int itemType)
{
    if (type == UndefType) return "undefined";
    else if (type == IntType) return "integer";
    else if (type == BoolType) return "boolean";
    else if (type == FloatType) return "real";
    else if (type == ColorType) return "color";
    else if (type == VersionType) return "version";
    else if (type == ObjectType) return "object";
    else if (type == StringType) return "string";
    else if (type == AnyType) return "any";
    else if (type == ListType) {
        if (itemType == IntType) return "list of integer";
        else if (itemType == BoolType) return "list of boolean";
        else if (itemType == FloatType) return "list of real";
        else if (itemType == ColorType) return "list of color";
        else if (itemType == VersionType) return "list of version";
        else if (itemType == ObjectType) return "list of object";
        else if (itemType == StringType) return "list of string";
        else return "list";
    }
    else if (type == MapType) {
        return "map";
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

bool Variant::operator==(const Variant &b) const
{
    bool equal = false;

    if ((type_ & IntType) && (b.type_ & IntType))
        equal = (int_ == b.int_);
    else if ((type_ & FloatType) && (b.type_ & FloatType))
        equal = (float_ == b.float_);
    else if ((type_ & ColorType) && (b.type_ & ColorType))
        equal = (word_ == b.word_);
    else if ((type_ & VersionType) && (b.type_ & VersionType))
        equal = (word_ == b.word_);
    else if ((type_ == StringType) && (b.type_ == StringType))
        equal = Variant::cast<CharArray *>(*this)->equals(Variant::cast<CharArray *>(b));
    else if ((type_ == ObjectType) && (b.type_ == ObjectType))
        equal = (ref().get() == b.ref().get());
    else
        equal = ((type_ == UndefType) && (b.type_ == UndefType));

    return equal;
}

bool Variant::operator<(const Variant &b) const
{
    bool below = false;

    if ((type_ & IntType) && (b.type_ & IntType))
        below = (int_ < b.int_);
    else if ((type_ & FloatType) && (b.type_ & FloatType))
        below = (float_ < b.float_);
    else if ((type_ & VersionType) && (b.type_ & VersionType))
        below = (Version::fromWord(word_) < Version::fromWord(b.word_));
    else if ((type_ == StringType) && (b.type_ == StringType))
        below = Variant::cast<CharArray *>(*this)->below(Variant::cast<CharArray *>(b));
    else if ((type_ == ObjectType) && (b.type_ == ObjectType))
        below = (ref().get() < b.ref().get());

    return below;
}

String str(const Variant &x)
{
    if (Variant::type(x) == Variant::UndefType) return "";
    else if (Variant::type(x) == Variant::BoolType) return str(bool(x));
    else if (Variant::type(x) == Variant::IntType) return str(int(x));
    else if (Variant::type(x) == Variant::FloatType) return str(float(x));
    else if (Variant::type(x) == Variant::ColorType) return str(Color(x));
    else if (Variant::type(x) == Variant::VersionType) return str(Version(x));
    else if (Variant::type(x) == Variant::StringType) return Variant::cast<CharArray *>(x);
    /*else if (type(x) == Variant::ObjectType)*/ return str((void *)Variant::cast<Object *>(x));
}

} // namespace cc
