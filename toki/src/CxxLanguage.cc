/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/toki/Registry>
#include <cc/toki/CppDiscoverySyntax>
#include <cc/toki/CxxSyntax>

namespace cc {
namespace toki {

class CxxLanguage: public Language
{
private:
    friend class Registration<CxxLanguage>;

    CxxLanguage():
        Language(
            "C++",
            RegExp("*.((h|c){0..1:(pp|xx|++|h|c)}|(m|mm))"),
            cxxSyntax(),
            cppDiscoverySyntax()
        )
    {}
};

namespace { Registration<CxxLanguage> registration; }

}} // namespace cc::toki