/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_SYNTAXDEBUGFACTORY_H
#define FLUX_SYNTAXDEBUGFACTORY_H

#include <flux/SyntaxNode>

namespace flux {

namespace syntax {

class DefinitionNode;

class DebugFactory: public Object
{
public:
    virtual Node *produce(Node *newNode, const char *nodeType) = 0;

protected:
    DefinitionNode *definition() const;

private:
    friend class DefinitionNode;
    DefinitionNode *definition_;
};

} // namespace syntax

typedef syntax::DebugFactory SyntaxDebugFactory;

} // namespace flux

#endif // FLUX_SYNTAXDEBUGFACTORY_H
