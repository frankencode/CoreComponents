/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/syntax/SyntaxDefinition>
#include <cc/regexp/RegExp>

namespace cc {
namespace toki {

using namespace cc::syntax;
using namespace cc::regexp;

class Language: public Object
{
public:
    inline String displayName() const { return displayName_; }
    inline String name() const { return name_; }
    inline RegExp pathPattern() const { return pathPattern_; }
    inline const SyntaxDefinition *highlightingSyntax() const { return highlightingSyntax_; }
    inline const SyntaxDefinition *discoverySyntax() const { return discoverySyntax_; }
    inline const SyntaxDefinition *foldingSyntax() const { return foldingSyntax_; }

protected:
    Language(
        String displayName,
        RegExp pathPattern,
        const SyntaxDefinition *highlightingSyntax,
        const SyntaxDefinition *discoverySyntax = 0,
        const SyntaxDefinition *foldingSyntax = 0
    );

private:
    Language();

    String displayName_;
    String name_;
    RegExp pathPattern_;
    Ref<const SyntaxDefinition> highlightingSyntax_;
    Ref<const SyntaxDefinition> discoverySyntax_;
    Ref<const SyntaxDefinition> foldingSyntax_;
};

}} // namespace cc::toki

