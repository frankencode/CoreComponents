/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
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

const TypeInfo VariantType<Version>::info
{
    .typeName = "Version",
    .str = [](const void *bytes) {
        return VariantType<Version>::retrieve(bytes).toString();
    },
    .cleanup = [](void *bytes){},
    .assign = [](void *dst, const void *src) {
        new(dst)Version{*static_cast<const Version *>(src)};
    },
    .equal = [](const void *a, const void *b) {
        return VariantType<Version>::retrieve(a) == VariantType<Version>::retrieve(b);
    }
};

} // namespace cc
