/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/Singleton>
#include <flux/toki/PlaintextSyntax>

namespace flux {
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

}} // namespace flux::toki
