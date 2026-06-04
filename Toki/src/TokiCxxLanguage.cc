/*
 * Copyright (C) 2007-2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the Apache License version 2.0
 * (see CoreComponents/LICENSE-Apache-2.0).
 *
 */

#include <cc/TokiLanguage>
#include <cc/TokiCppDiscoverySyntax>
#include <cc/TokiCxxSyntax>
#include <cc/REGISTRATION>

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
