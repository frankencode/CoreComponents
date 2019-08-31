/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/toki/ScriptDiscoverySyntax>

namespace cc {
namespace toki {

Ref<ScriptDiscoverySyntax> ScriptDiscoverySyntax::create(const char *suffix)
{
    return new ScriptDiscoverySyntax{suffix};
}

ScriptDiscoverySyntax::ScriptDiscoverySyntax(const char *suffix)
{
    DEFINE("FirstLine",
        GLUE(
            STRING("#!"),
            LAZY_REPEAT(OTHER('\n')),
            STRING(suffix)
        )
    );

    ENTRY("FirstLine");
    LINK();
}

}} // namespace cc::toki
