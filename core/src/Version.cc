/*
 * Copyright (C) 2007-2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Format>
#include <cc/Version>

namespace cc {

Version::Version(const Variant &v)
{
    *this = Variant::cast<Version>(v);
}

Version::Instance::Instance(const String &s)
{
    Ref<StringList> parts = s->trim("v")->split('.');
    if (parts->has(0)) major_ = parts->at(0)->toNumber<int>();
    if (parts->has(1)) minor_ = parts->at(1)->toNumber<int>();
    if (parts->has(2)) patch_ = parts->at(2)->toNumber<int>();
}

String Version::Instance::toString() const
{
    return Format("%%.%%.%%") << major_ << minor_ << patch_;
}

} // namespace cc
