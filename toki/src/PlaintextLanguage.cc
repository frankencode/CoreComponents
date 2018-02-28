/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/toki/Registry>
#include <cc/toki/PlaintextSyntax>

namespace cc {
namespace toki {

class PlaintextLanguage: public Language
{
private:
    friend class Registration<PlaintextLanguage>;

    PlaintextLanguage():
        Language(
            "Plaintext",
            Pattern("*.txt"),
            plaintextSyntax()
        )
    {}
};

namespace { Registration<PlaintextLanguage> registration; }

}} // namespace cc::toki
