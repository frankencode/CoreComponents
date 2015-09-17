/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXSYNTAX_SYNTAXDEBUGFACTORY_H
#define FLUXSYNTAX_SYNTAXDEBUGFACTORY_H

#include <flux/syntax/SyntaxNode>

namespace flux {
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

}} // namespace flux::syntax

#endif // FLUXSYNTAX_SYNTAXDEBUGFACTORY_H
