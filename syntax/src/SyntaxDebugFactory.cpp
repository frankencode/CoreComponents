/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/syntax/syntax>
#include <flux/syntax/SyntaxDebugFactory>

namespace flux {
namespace syntax {

DefinitionNode *SyntaxDebugFactory::definition() const { return definition_; }

}} // namespace flux::syntax
