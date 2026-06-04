/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the Apache License version 2.0
 * (see CoreComponents/LICENSE-Apache-2.0).
 *
 */

#include <cc/TokiHighlightingSyntax>

namespace cc {

void TokiHighlightingSyntax::clearMetaData()
{
    for (KeyValue<String, SyntaxRule> &pair: me().ruleByName_)
    {
        pair.value().setMetaData(Object{});
    }
}

} // namespace cc
