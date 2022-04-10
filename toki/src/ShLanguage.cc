/*
 * Copyright (C) 2007-2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/toki/Language>
#include <cc/toki/ShDiscoverySyntax>
#include <cc/toki/ShSyntax>
#include <cc/Registration>

namespace cc::toki {

class ShLanguage final: public Language
{
public:
    ShLanguage():
        Language{
            "Shell",
            "sh",
            Pattern{"*.sh"},
            ShSyntax{},
            ShDiscoverySyntax{}
        }
    {}
};

CC_REGISTRATION(ShLanguage)

} // namespace cc::toki
