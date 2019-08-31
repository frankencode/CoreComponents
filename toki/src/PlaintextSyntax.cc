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

const PlaintextSyntax *PlaintextSyntax::instance()
{
    return Singleton<PlaintextSyntax>::instance();
}

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

}} // namespace cc::toki
