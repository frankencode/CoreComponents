/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Format>
#include <cc/Version>

namespace cc {

Version::Version(const char* s):
    major_(0),
    minor_(0),
    patch_(0)
{
    read(s);
}

Version::Version(const String &s):
    major_(0),
    minor_(0),
    patch_(0)
{
    read(s);
}

Version::Version(const Variant &v)
{
    *this = Variant::toVersion(v);
}

void Version::read(String s)
{
    Ref<StringList> parts = s->trim("v")->split('.');
    if (parts->has(0)) major_ = parts->at(0)->toNumber<int>();
    if (parts->has(1)) minor_ = parts->at(1)->toNumber<int>();
    if (parts->has(2)) patch_ = parts->at(2)->toNumber<int>();
}

String str(Version v)
{
    return Format("%%.%%.%%") << Version::major(v) << Version::minor(v) << Version::patch(v);
}

} // namespace cc
