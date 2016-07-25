/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/syntax/SyntaxDefinition>

namespace cc {
namespace toki {

using namespace cc::syntax;

class ScriptDiscoverySyntax: public SyntaxDefinition
{
public:
    static Ref<ScriptDiscoverySyntax> create(const char *suffix);

protected:
    ScriptDiscoverySyntax(const char *suffix);
};

}} // namespace cc::toki

