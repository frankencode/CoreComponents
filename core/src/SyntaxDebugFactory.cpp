/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/syntax>
#include <flux/SyntaxDebugFactory>

namespace flux {

namespace syntax {

DefinitionNode *DebugFactory::definition() const { return definition_; }

} // namespace syntax

} // namespace flux
