/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/Variant>
#include <cc/input>
#include <cc/str>

namespace cc {

const TypeInfo VariantType<void>::info
{
    .typeName = "void",
    .str = [](const void *bytes) { return String{}; }
};

const TypeInfo VariantType<long>::info
{
    .typeName = "long",
    .str = [](const void *bytes) { return str(VariantType<long>::retrieve(bytes)); },
    .equal = [](const void *a, const void *b) {
        return VariantType<long>::retrieve(a) == VariantType<long>::retrieve(b);
    },
    .order = [](const void *a, const void *b) {
        return VariantType<long>::retrieve(a) <=> VariantType<long>::retrieve(b);
    }
};

const TypeInfo VariantType<bool>::info
{
    .typeName = "bool",
    .str = [](const void *bytes) { return str(VariantType<bool>::retrieve(bytes)); },
    .equal = [](const void *a, const void *b) {
        return VariantType<bool>::retrieve(a) == VariantType<bool>::retrieve(b);
    },
    .order = [](const void *a, const void *b) {
        return VariantType<bool>::retrieve(a) <=> VariantType<bool>::retrieve(b);
    }
};

const TypeInfo VariantType<double>::info
{
    .typeName = "double",
    .str = [](const void *bytes) { return str(VariantType<double>::retrieve(bytes)); },
    .equal = [](const void *a, const void *b) {
        return VariantType<double>::retrieve(a) == VariantType<double>::retrieve(b);
    }
};

const TypeInfo VariantType<String>::info {
    .typeName = "String",
    .str = [](const void *bytes) {
        return String{VariantType<String>::retrieve(bytes)};
    },
    .cleanup = [](void *bytes){
        static_cast<String *>(bytes)->~String();
    },
    .assign = [](void *dst, const void *src) {
        new(dst)String{*static_cast<const String *>(src)};
    },
    .equal = [](const void *a, const void *b) {
        return VariantType<String>::retrieve(a) == VariantType<String>::retrieve(b);
    },
    .order = [](const void *a, const void *b) {
        return VariantType<String>::retrieve(a) <=> VariantType<String>::retrieve(b);
    }
};

const TypeInfo VariantType<List<Variant>>::info {
    .typeName = "List<Variant>",
    .str = [](const void *bytes) {
        const List<Variant> &list = *static_cast<const List<Variant> *>(bytes);
        if (list.count() == 0) return String{"[]"};
        List<String> parts;
        parts << "[ ";
        for (Locator pos = list.head(); pos; ++pos) {
            Variant value = list.at(pos);
            if (value.is<String>()) parts << "\"";
            parts << value;
            if (value.is<String>()) parts << "\"";
            if (+pos != list.count() - 1)
                parts << ", ";
        }
        parts << " ]";
        return String{parts};
    },
    .cleanup = [](void *bytes) {
        static_cast<List<Variant> *>(bytes)->~List<Variant>();
    },
    .assign = [](void *dst, const void *src) {
        new(dst)List<Variant>{*static_cast<const List<Variant> *>(src)};
    },
    .equal = [](const void *a, const void *b) {
        return VariantType<List<Variant>>::retrieve(a) == VariantType<List<Variant>>::retrieve(b);
    },
    .order = [](const void *a, const void *b) {
        return VariantType<List<Variant>>::retrieve(a) <=> VariantType<List<Variant>>::retrieve(b);
    }
};

const TypeInfo VariantType<List<String>>::info {
    .typeName = "List<String>",
    .str = [](const void *bytes) {
        const List<String> &list = *static_cast<const List<String> *>(bytes);
        if (list.count() == 0) return String{"[]"};
        return String{"[ \"" + String{list, "\", \""} + "\" ]"};
    },
    .cleanup = [](void *bytes) {
        static_cast<List<String> *>(bytes)->~List<String>();
    },
    .assign = [](void *dst, const void *src) {
        new(dst)List<String>{*static_cast<const List<String> *>(src)};
    },
    .equal = [](const void *a, const void *b) {
        return VariantType<List<String>>::retrieve(a) == VariantType<List<String>>::retrieve(b);
    },
    .order = [](const void *a, const void *b) {
        return VariantType<List<String>>::retrieve(a) <=> VariantType<List<String>>::retrieve(b);
    }
};

const TypeInfo VariantType<List<bool>>::info {
    .typeName = "List<bool>",
    .str = [](const void *bytes) {
        const List<bool> &list = *static_cast<const List<bool> *>(bytes);
        if (list.count() == 0) return String{"[]"};
        List<String> parts;
        parts << "[ ";
        for (Locator pos = list.head(); pos; ++pos) {
            parts << str(list.at(pos));
            if (+pos != list.count() - 1)
                parts << ", ";
        }
        parts << " ]";
        return String{parts};
    },
    .cleanup = [](void *bytes) {
        static_cast<List<bool> *>(bytes)->~List<bool>();
    },
    .assign = [](void *dst, const void *src) {
        new(dst)List<bool>{*static_cast<const List<bool> *>(src)};
    },
    .equal = [](const void *a, const void *b) {
        return VariantType<List<bool>>::retrieve(a) == VariantType<List<bool>>::retrieve(b);
    },
    .order = [](const void *a, const void *b) {
        return VariantType<List<bool>>::retrieve(a) <=> VariantType<List<bool>>::retrieve(b);
    }
};

const TypeInfo VariantType<List<long>>::info {
    .typeName = "List<long>",
    .str = [](const void *bytes) {
        const List<long> &list = *static_cast<const List<long> *>(bytes);
        if (list.count() == 0) return String{"[]"};
        List<String> parts;
        parts << "[ ";
        for (Locator pos = list.head(); pos; ++pos) {
            parts << str(list.at(pos));
            if (+pos != list.count() - 1)
                parts << ", ";
        }
        parts << " ]";
        return String{parts};
    },
    .cleanup = [](void *bytes) {
        static_cast<List<long> *>(bytes)->~List<long>();
    },
    .assign = [](void *dst, const void *src) {
        new(dst)List<long>{*static_cast<const List<long> *>(src)};
    },
    .equal = [](const void *a, const void *b) {
        return VariantType<List<long>>::retrieve(a) == VariantType<List<long>>::retrieve(b);
    },
    .order = [](const void *a, const void *b) {
        return VariantType<List<long>>::retrieve(a) <=> VariantType<List<long>>::retrieve(b);
    }
};

const TypeInfo VariantType<List<double>>::info {
    .typeName = "List<double>",
    .str = [](const void *bytes) {
        const List<double> &list = *static_cast<const List<double> *>(bytes);
        if (list.count() == 0) return String{"[]"};
        List<String> parts;
        parts << "[ ";
        for (Locator pos = list.head(); pos; ++pos) {
            parts << str(list.at(pos));
            if (+pos != list.count() - 1)
                parts << ", ";
        }
        parts << " ]";
        return String{parts};
    },
    .cleanup = [](void *bytes) {
        static_cast<List<double> *>(bytes)->~List<double>();
    },
    .assign = [](void *dst, const void *src) {
        new(dst)List<double>{*static_cast<const List<double> *>(src)};
    },
    .equal = [](const void *a, const void *b) {
        return VariantType<List<double>>::retrieve(a) == VariantType<List<double>>::retrieve(b);
    }
};

const TypeInfo VariantType<Map<String, Variant>>::info {
    .typeName = "Map<String,Variant>",
    .str = [](const void *bytes) {
        const Map<String, Variant> &map = *static_cast<const Map<String, Variant> *>(bytes);
        List<String> parts;
        parts << "{ ";
        for (Locator pos = map.head(); pos; ++pos) {
            Variant value = map.at(pos).value();
            parts << "\"" << map.at(pos).key() << "\": ";
            if (value.is<String>()) parts << "\"";
            parts << str(value);
            if (value.is<String>()) parts << "\"";
            if (+pos != map.count() - 1)
                parts << ", ";
        }
        parts << " }";
        return String{parts};
    },
    .cleanup = [](void *bytes) {
        static_cast<Map<String, Variant> *>(bytes)->~Map<String, Variant>();
    },
    .assign = [](void *dst, const void *src) {
        new(dst)Map<String, Variant>{*static_cast<const Map<String, Variant> *>(src)};
    },
    .equal = [](const void *a, const void *b) {
        return VariantType<Map<String, Variant>>::retrieve(a) == VariantType<Map<String, Variant>>::retrieve(b);
    }
};

void VariantType<Map<String, Variant>>::store(void *bytes, const Map<String, Variant> &x)
{
    static_assert(sizeof(Map<String, Variant>) <= 8);

    new(bytes)Map<String, Variant>{x};
}

Map<String, Variant> VariantType<Map<String, Variant>>::retrieve(const void *bytes, const TypeInfo &typeInfo)
{
    assert((&typeInfo == &VariantType<Map<String, Variant>>::info)); // expected type 'Map<String, Variant>'

    return *static_cast<const Map<String, Variant> *>(bytes);
}

Variant Variant::read(const String &s)
{
    if (s.startsWith('"') && s.endsWith('"'))
        return Variant{s.copy(1, s.count() - 1)};

    if (s.startsWith('[') && s.endsWith(']')) {
        List<String> sl = s.copy(1, s.count() - 1).split(',');
        List<Variant> vl;
        for (String &x: sl) {
            x.trim();
            vl.append(Variant::read(x));
        }
        return Variant{vl};
    }

    if (
        s.find('.') ||
        (!s.startsWith("0x") && (s.find('e') || s.find('E')))
    ) {
        double value = 0;
        if (scanNumber<double>(s, &value) == s.count())
            return Variant{value};
    }

    long value = 0;
    if (scanNumber<long>(s, &value) == s.count()) return Variant{value};
    if (s == "true" || s == "on") return Variant{true};
    if (s == "false" || s == "off") return Variant{false};

    return Variant{s};
}

} // namespace cc
