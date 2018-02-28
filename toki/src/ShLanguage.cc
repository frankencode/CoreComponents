/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/toki/Registry>
#include <cc/toki/ShSyntax>
#include <cc/toki/ScriptDiscoverySyntax>

namespace cc {
namespace toki {

class ShLanguage: public Language
{
private:
    friend class Registration<ShLanguage>;

    ShLanguage():
        Language(
            "Sh",
            Pattern("*.sh"),
            shSyntax(),
            ScriptDiscoverySyntax::create("sh")
        )
    {}
};

namespace { Registration<ShLanguage> registration; }

}} // namespace cc::toki
