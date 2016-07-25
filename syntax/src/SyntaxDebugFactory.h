/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/syntax/SyntaxNode>

namespace cc {
namespace syntax {

class DefinitionNode;

class SyntaxDebugFactory: public Object
{
public:
    virtual SyntaxNode *produce(SyntaxNode *newNode, const char *nodeType) = 0;

protected:
    DefinitionNode *definition() const;

private:
    friend class DefinitionNode;
    DefinitionNode *definition_;
};

}} // namespace cc::syntax

