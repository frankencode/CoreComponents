/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
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
