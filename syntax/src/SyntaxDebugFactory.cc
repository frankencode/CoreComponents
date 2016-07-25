/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/syntax/syntax>
#include <cc/syntax/SyntaxDebugFactory>

namespace cc {
namespace syntax {

DefinitionNode *SyntaxDebugFactory::definition() const { return definition_; }

}} // namespace cc::syntax
