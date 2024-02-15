/*
 * Copyright (C) 2024 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/VariantType>
#include <cc/Variant>
#include <cc/str>

namespace cc {

struct VoidTypeInfo final: public TypeInfo
{
    const char *typeName() const override { return "void"; }
    String str(const void *bytes) const override { return {}; }
};

const TypeInfo &VariantType<void>::info()
{
    return global<VoidTypeInfo>();
}

struct LongTypeInfo final: public TypeInfo
{
    const char *typeName() const override { return "long"; }
    String str(const void *bytes) const override { return cc::str(VariantType<long>::retrieve(bytes)); }
    bool equal(const void *a, const void *b) const override { return VariantType<long>::retrieve(a) == VariantType<long>::retrieve(b); }
    std::strong_ordering order(const void *a, const void *b) const override { return VariantType<long>::retrieve(a) <=> VariantType<long>::retrieve(b); }
};

const TypeInfo &VariantType<long>::info()
{
    return global<LongTypeInfo>();
}

struct BoolTypeInfo final: public TypeInfo
{
    const char *typeName() const override { return "bool"; }
    String str(const void *bytes) const override { return cc::str(VariantType<bool>::retrieve(bytes)); }
    bool equal(const void *a, const void *b) const override { return VariantType<bool>::retrieve(a) == VariantType<bool>::retrieve(b); }
    std::strong_ordering order(const void *a, const void *b) const override { return VariantType<bool>::retrieve(a) <=> VariantType<bool>::retrieve(b); }
};

const TypeInfo &VariantType<bool>::info()
{
    return global<BoolTypeInfo>();
}

struct DoubleTypeInfo final: public TypeInfo
{
    const char *typeName() const override { return "double"; }

    String str(const void *bytes) const override { return cc::str(VariantType<double>::retrieve(bytes)); }

    bool equal(const void *a, const void *b) const override { return VariantType<double>::retrieve(a) == VariantType<double>::retrieve(b); }

    std::strong_ordering order(const void *a, const void *b) const override
    {
        double c = VariantType<double>::retrieve(a);
        double d = VariantType<double>::retrieve(b);
        if (c < d) return std::strong_ordering::less;
        else if (d < c) return std::strong_ordering::greater;
        return std::strong_ordering::equal;
    }
};

const TypeInfo &VariantType<double>::info()
{
    return global<DoubleTypeInfo>();
}

struct StringTypeInfo final: public TypeInfo
{
    const char *typeName() const override { return "String"; }

    String str(const void *bytes) const override { return VariantType<String>::retrieve(bytes); }

    void cleanup(void *bytes) const override { static_cast<String *>(bytes)->~String(); }

    void assign(void *dst, const void *src) const override { new(dst)String{*static_cast<const String *>(src)}; }

    bool equal(const void *a, const void *b) const override { return VariantType<String>::retrieve(a) == VariantType<String>::retrieve(b); }

    std::strong_ordering order(const void *a, const void *b) const override { return VariantType<String>::retrieve(a) <=> VariantType<String>::retrieve(b); }
};

const TypeInfo &VariantType<String>::info()
{
    return global<StringTypeInfo>();
}

struct BoolListTypeInfo final: public TypeInfo
{
    const char *typeName() const override { return "List<bool>"; }

    String str(const void *bytes) const override
    {
        const List<bool> &list = *static_cast<const List<bool> *>(bytes);
        if (list.count() == 0) return "[]";
        List<String> parts;
        parts << "[ ";
        for (Locator pos = list.head(); pos; ++pos) {
            parts << cc::str(list.at(pos));
            if (+pos != list.count() - 1)
                parts << ", ";
        }
        parts << " ]";
        return parts;
    }

    void cleanup(void *bytes) const override { static_cast<List<bool> *>(bytes)->~List<bool>(); }

    void assign(void *dst, const void *src) const override { new(dst)List<bool>{*static_cast<const List<bool> *>(src)}; }

    bool equal(const void *a, const void *b) const override { return VariantType<List<bool>>::retrieve(a) == VariantType<List<bool>>::retrieve(b); }

    std::strong_ordering order(const void *a, const void *b) const override { return VariantType<List<bool>>::retrieve(a) <=> VariantType<List<bool>>::retrieve(b); }
};

const TypeInfo &VariantType<List<bool>>::info()
{
    return global<BoolListTypeInfo>();
}

struct LongListTypeInfo final: public TypeInfo
{
    const char *typeName() const override { return "List<long>"; }

    String str(const void *bytes) const override
    {
        const List<long> &list = *static_cast<const List<long> *>(bytes);
        if (list.count() == 0) return "[]";
        List<String> parts;
        parts << "[ ";
        for (Locator pos = list.head(); pos; ++pos) {
            parts << cc::str(list.at(pos));
            if (+pos != list.count() - 1)
                parts << ", ";
        }
        parts << " ]";
        return parts;
    }

    void cleanup(void *bytes) const override { static_cast<List<long> *>(bytes)->~List<long>(); }

    void assign(void *dst, const void *src) const override { new(dst)List<long>{*static_cast<const List<long> *>(src)}; }

    bool equal(const void *a, const void *b) const override { return VariantType<List<long>>::retrieve(a) == VariantType<List<long>>::retrieve(b); }

    std::strong_ordering order(const void *a, const void *b) const override { return VariantType<List<long>>::retrieve(a) <=> VariantType<List<long>>::retrieve(b); }
};

const TypeInfo &VariantType<List<long>>::info()
{
    return global<LongListTypeInfo>();
}

struct DoubleListTypeInfo final: public TypeInfo
{
    const char *typeName() const override { return "List<double>"; }

    String str(const void *bytes) const override
    {
        const List<double> &list = *static_cast<const List<double> *>(bytes);
        if (list.count() == 0) return "[]";
        List<String> parts;
        parts << "[ ";
        for (Locator pos = list.head(); pos; ++pos) {
            parts << cc::str(list.at(pos));
            if (+pos != list.count() - 1)
                parts << ", ";
        }
        parts << " ]";
        return parts;
    }

    void cleanup(void *bytes) const override {  static_cast<List<double> *>(bytes)->~List<double>(); }

    void assign(void *dst, const void *src) const override { new(dst)List<double>{*static_cast<const List<double> *>(src)}; }

    bool equal(const void *a, const void *b) const override { return VariantType<List<double>>::retrieve(a) == VariantType<List<double>>::retrieve(b); }

    std::strong_ordering order(const void *a, const void *b) const override
    {
        const List<double> c = VariantType<List<double>>::retrieve(a);
        const List<double> d = VariantType<List<double>>::retrieve(b);
        if (c < d) return std::strong_ordering::less;
        if (d < c) return std::strong_ordering::greater;
        return std::strong_ordering::equal;
    }
};

const TypeInfo &VariantType<List<double>>::info()
{
    return global<DoubleListTypeInfo>();
}

struct StringListTypeInfo final: public TypeInfo
{
    const char *typeName() const override { return "List<String>"; }

    String str(const void *bytes) const override
    {
        const List<String> &list = *static_cast<const List<String> *>(bytes);
        if (list.count() == 0) return String{"[]"};
        return String{"[ \"" + String{list, "\", \""} + "\" ]"};
    }

    void cleanup(void *bytes) const override { static_cast<List<String> *>(bytes)->~List<String>(); }

    void assign(void *dst, const void *src) const override { new(dst)List<String>{*static_cast<const List<String> *>(src)}; }

    bool equal(const void *a, const void *b) const override { return VariantType<List<String>>::retrieve(a) == VariantType<List<String>>::retrieve(b); }

    std::strong_ordering order(const void *a, const void *b) const override { return VariantType<List<String>>::retrieve(a) <=> VariantType<List<String>>::retrieve(b); }
};

const TypeInfo &VariantType<List<String>>::info()
{
    return global<StringListTypeInfo>();
}

struct VariantListTypeInfo final: public TypeInfo
{
    const char *typeName() const override { return "List<Variant>"; }

    String str(const void *bytes) const override
    {
        const List<Variant> &list = *static_cast<const List<Variant> *>(bytes);
        if (list.count() == 0) return "[]";
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
        return parts;
    }

    void cleanup(void *bytes) const override { static_cast<List<Variant> *>(bytes)->~List<Variant>(); }

    void assign(void *dst, const void *src) const override { new(dst)List<Variant>{*static_cast<const List<Variant> *>(src)}; }

    bool equal(const void *a, const void *b) const override { return VariantType<List<Variant>>::retrieve(a) == VariantType<List<Variant>>::retrieve(b); }

    std::strong_ordering order(const void *a, const void *b) const override { return VariantType<List<Variant>>::retrieve(a) <=> VariantType<List<Variant>>::retrieve(b); }
};

const TypeInfo &VariantType<List<Variant>>::info()
{
    return global<VariantListTypeInfo>();
}

void VariantType<List<Variant>>::store(void *bytes, const List<Variant> &x)
{
    static_assert(sizeof(List<Variant>) <= 8);

    new(bytes)List<Variant>{x};
}

List<Variant> VariantType<List<Variant>>::retrieve(const void *bytes, const TypeInfo &typeInfo)
{
    assert(&typeInfo == &VariantType<List<Variant>>::info()); // expected type 'List<Variant>'

    return *static_cast<const List<Variant> *>(bytes);
}

struct VariantMapTypeInfo final: public TypeInfo
{
    const char *typeName() const override { return "Map<String,Variant>"; }

    String str(const void *bytes) const override
    {
        const Map<String, Variant> &map = *static_cast<const Map<String, Variant> *>(bytes);
        if (map.count() == 0) return "{}";
        List<String> parts;
        parts << "{ ";
        for (Locator pos = map.head(); pos; ++pos) {
            Variant value = map.at(pos).value();
            parts << "\"" << map.at(pos).key() << "\": ";
            if (value.is<String>()) parts << "\"";
            parts << cc::str(value);
            if (value.is<String>()) parts << "\"";
            if (+pos != map.count() - 1)
                parts << ", ";
        }
        parts << " }";
        return parts;
    }

    void cleanup(void *bytes) const override { static_cast<Map<String, Variant> *>(bytes)->~Map<String, Variant>(); }

    void assign(void *dst, const void *src) const override { new(dst)Map<String, Variant>{*static_cast<const Map<String, Variant> *>(src)}; }

    bool equal(const void *a, const void *b) const override { return VariantType<Map<String, Variant>>::retrieve(a) == VariantType<Map<String, Variant>>::retrieve(b); }
};

const TypeInfo &VariantType<Map<String, Variant>>::info()
{
    return global<VariantMapTypeInfo>();
}

void VariantType<Map<String, Variant>>::store(void *bytes, const Map<String, Variant> &x)
{
    static_assert(sizeof(Map<String, Variant>) <= 8);

    new(bytes)Map<String, Variant>{x};
}

Map<String, Variant> VariantType<Map<String, Variant>>::retrieve(const void *bytes, const TypeInfo &typeInfo)
{
    assert((&typeInfo == &VariantType<Map<String, Variant>>::info())); // expected type 'Map<String, Variant>'

    return *static_cast<const Map<String, Variant> *>(bytes);
}

} // namespace cc
