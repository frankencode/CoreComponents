/*
 * Copyright (C) 2007-2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/TokiLanguage>
#include <cc/TokiShDiscoverySyntax>
#include <cc/TokiShSyntax>
#include <cc/REGISTRATION>

namespace cc {

class ShLanguage final: public TokiLanguage
{
public:
    ShLanguage():
        TokiLanguage{
            "Shell",
            "sh",
            Pattern{"*.sh"},
            TokiShSyntax{},
            TokiShDiscoverySyntax{}
        }
    {}
};

CC_REGISTRATION(ShLanguage)

} // namespace cc
