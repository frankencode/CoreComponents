/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/Singleton>
#include <flux/toki/YasonSyntax>

namespace flux {
namespace toki {

class YasonSyntax: public flux::meta::YasonSyntax
{
protected:
    friend class flux::Singleton<YasonSyntax>;

    YasonSyntax()
        : flux::meta::YasonSyntax(GenerateComments|GenerateEscapedChars)
    {}
};

flux::meta::YasonSyntax *yasonSyntax() { return flux::Singleton<flux::toki::YasonSyntax>::instance(); }

}} // namespace flux::toki
