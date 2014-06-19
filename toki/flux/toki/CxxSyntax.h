/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXTOKI_CXXSYNTAX_H
#define FLUXTOKI_CXXSYNTAX_H

#include <flux/SyntaxDefinition.h>

namespace flux {

template<class> class Singleton;

namespace toki {

class CxxSyntax: public SyntaxDefinition
{
public:
protected:
	friend class Singleton<CxxSyntax>;
	CxxSyntax();
};

const CxxSyntax *cxxSyntax();

}} // namespace flux::toki

#endif // FLUXTOKI_CXXSYNTAX_H
