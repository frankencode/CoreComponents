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

class CppDiscoverySyntax: public SyntaxDefinition
{
private:
    friend class Singleton<CppDiscoverySyntax>;
    CppDiscoverySyntax();
};

CppDiscoverySyntax::CppDiscoverySyntax()
{
    SYNTAX("cpp_discovery");
    IMPORT(cxxSyntax(), "cxx");

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

const SyntaxDefinition *cppDiscoverySyntax() { return Singleton<CppDiscoverySyntax>::instance(); }

}} // namespace cc::toki
