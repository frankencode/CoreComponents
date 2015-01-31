/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/toki/Registry>
#include <flux/toki/PlaintextSyntax>

namespace flux {
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

}} // namespace flux::toki
