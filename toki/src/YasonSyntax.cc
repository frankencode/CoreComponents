/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Singleton>
#include <cc/toki/YasonSyntax>

namespace cc {
namespace toki {

const YasonSyntax *YasonSyntax::instance()
{
    return Singleton<YasonSyntax>::instance();
}

YasonSyntax::YasonSyntax():
    cc::meta::YasonSyntax{GenerateComments|GenerateEscapedChars}
{}

}} // namespace cc::toki
