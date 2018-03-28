/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/toki/Registry>
#include <cc/toki/YasonSyntax>

namespace cc {
namespace toki {

class YasonLanguage: public Language
{
private:
    friend class Object;

    YasonLanguage():
        Language(
            "Yason",
            Pattern("(*.(yason|json))|Recipe"),
            cc::toki::yasonSyntax()
        )
    {}
};

namespace { Registration<YasonLanguage> registration; }

}} // namespace cc::toki
