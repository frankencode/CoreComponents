/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/Version>
#include <cc/input>
#include <cc/str>

namespace cc {

Version::Version(const String &s)
{
    List<String> parts = s.trimmed("v").split('.');
    if (parts.count() > 0) me().major = readNumber<int>(parts.at(0));
    if (parts.count() > 1) me().minor = readNumber<int>(parts.at(1));
    if (parts.count() > 2) me().patch = readNumber<int>(parts.at(2));
}

String Version::toString() const
{
    return String{
        List<String>{} << str(major()) << str(minor()) << str(patch()),
        "."
    };
}

struct VersionTypeInfo final: public TypeInfo
{
    const char *typeName() const override { return "Version"; };

    String str(const void *bytes) const override { return VariantType<Version>::retrieve(bytes).toString(); }

    void assign(void *dst, const void *src) const override { new(dst)Version{*static_cast<const Version *>(src)}; }

    bool equal(const void *a, const void *b) const override { return VariantType<Version>::retrieve(a) == VariantType<Version>::retrieve(b); }

    std::strong_ordering order(const void *a, const void *b) const override { return VariantType<Version>::retrieve(a) <=> VariantType<Version>::retrieve(b); }
};

const TypeInfo &VariantType<Version>::info()
{
    return global<VersionTypeInfo>();
}

} // namespace cc
