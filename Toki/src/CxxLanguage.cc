/*
 * Copyright (C) 2007-2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/toki/Language>
#include <cc/toki/CppDiscoverySyntax>
#include <cc/toki/CxxSyntax>
#include <cc/Registration>

namespace cc::toki {

class CxxLanguage final: public Language
{
public:
    CxxLanguage():
        Language{
            "C++",
            "cxx",
            Pattern{"*.((h|c){0..1:(pp|xx|++|h|c)}|(m|mm))"},
            CxxSyntax{},
            CppDiscoverySyntax{}
        }
    {}
};

CC_REGISTRATION(CxxLanguage)

} // namespace cc::toki
