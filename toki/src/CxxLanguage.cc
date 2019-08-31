/*
 * Copyright (C) 2007-2017 Frank Mertens.
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
    friend class Object;

    CxxLanguage():
        Language{
            "C++",
            Pattern{"*.((h|c){0..1:(pp|xx|++|h|c)}|(m|mm))"},
            CxxSyntax::instance(),
            CppDiscoverySyntax::instance()
        }
    {}
};

namespace { Registration<CxxLanguage> registration; }

}} // namespace cc::toki
