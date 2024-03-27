/*
 * Copyright (C) 2007-2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/TokiLanguage>
#include <cc/TokiCppDiscoverySyntax>
#include <cc/TokiCxxSyntax>
#include <cc/Registration>

namespace cc {

class CxxLanguage final: public TokiLanguage
{
public:
    CxxLanguage():
        TokiLanguage{
            "C++",
            "cxx",
            Pattern{"*.((h|c){0..1:(pp|xx|++|h|c)}|(m|mm))"},
            TokiCxxSyntax{},
            TokiCppDiscoverySyntax{}
        }
    {}
};

CC_REGISTRATION(CxxLanguage)

} // namespace cc
