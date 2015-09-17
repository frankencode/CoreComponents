/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXTOKI_SCRIPTDISCOVERYSYNTAX_H
#define FLUXTOKI_SCRIPTDISCOVERYSYNTAX_H

#include <flux/syntax/SyntaxDefinition>

namespace flux {
namespace toki {

using namespace flux::syntax;

class ScriptDiscoverySyntax: public SyntaxDefinition
{
public:
    static Ref<ScriptDiscoverySyntax> create(const char *suffix);

protected:
    ScriptDiscoverySyntax(const char *suffix);
};

}} // namespace flux::toki

#endif // FLUXTOKI_SCRIPTDISCOVERYSYNTAX_H
