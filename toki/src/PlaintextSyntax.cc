/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Singleton>
#include <cc/toki/PlaintextSyntax>

namespace cc {
namespace toki {

class PlaintextSyntax: public SyntaxDefinition
{
protected:
    friend class Singleton<PlaintextSyntax>;
    PlaintextSyntax();
};

PlaintextSyntax::PlaintextSyntax()
{
    SYNTAX("plaintext");

    DEFINE("Plaintext",
        REPEAT(
            ANY()
        )
    );

    ENTRY("Plaintext");
    LINK();
}

const SyntaxDefinition *plaintextSyntax()
{
    return Singleton<PlaintextSyntax>::instance();
}

}} // namespace cc::toki
