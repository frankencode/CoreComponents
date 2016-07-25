/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Singleton>
#include <cc/toki/YasonSyntax>

namespace cc {
namespace toki {

class YasonSyntax: public cc::meta::YasonSyntax
{
protected:
    friend class cc::Singleton<YasonSyntax>;

    YasonSyntax()
        : cc::meta::YasonSyntax(GenerateComments|GenerateEscapedChars)
    {}
};

cc::meta::YasonSyntax *yasonSyntax() { return cc::Singleton<cc::toki::YasonSyntax>::instance(); }

}} // namespace cc::toki
