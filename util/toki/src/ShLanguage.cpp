/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/toki/Registry>
#include <flux/toki/ShSyntax>
#include <flux/toki/ScriptDiscoverySyntax>

namespace flux {
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

}} // namespace flux::toki
