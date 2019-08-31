/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Singleton>
#include <cc/toki/CxxSyntax>
#include <cc/toki/CppDiscoverySyntax>

namespace cc {
namespace toki {

const CppDiscoverySyntax *CppDiscoverySyntax::instance()
{
    return Singleton<CppDiscoverySyntax>::instance();
}

CppDiscoverySyntax::CppDiscoverySyntax()
{
    SYNTAX("cpp_discovery");
    IMPORT(CxxSyntax::instance(), "cxx");

    DEFINE("Header",
        GLUE(
            REPEAT(
                CHOICE(
                    REPEAT(1, RANGE(" \n\t\r")),
                    INLINE("cxx::CommentText"),
                    INLINE("cxx::CommentLine")
                )
            ),
            CHAR('#'),
            RANGE(" \t"),
            KEYWORD("ifndef include")
        )
    );

    ENTRY("Header");
    LINK();
}

}} // namespace cc::toki
