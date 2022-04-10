/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/toki/HighlightingSyntax>

namespace cc::toki {

void HighlightingSyntax::clearMetaData()
{
    for (KeyValue<String, SyntaxRule> &pair: me().ruleByName_)
    {
        pair.value().setMetaData(Object{});
    }
}

} // namespace cc::toki
