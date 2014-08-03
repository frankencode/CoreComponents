/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXTOKI_SCRIPTDISCOVERYSYNTAX_H
#define FLUXTOKI_SCRIPTDISCOVERYSYNTAX_H

#include <flux/SyntaxDefinition.h>

namespace flux {
namespace toki {

class ScriptDiscoverySyntax: public SyntaxDefinition
{
public:
	static Ref<ScriptDiscoverySyntax> create(const char *suffix);

protected:
	ScriptDiscoverySyntax(const char *suffix);
};

}} // namespace flux::toki

#endif // FLUXTOKI_SCRIPTDISCOVERYSYNTAX_H
