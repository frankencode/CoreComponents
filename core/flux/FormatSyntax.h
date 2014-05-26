/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_FORMATSYNTAX_H
#define FLUX_FORMATSYNTAX_H

#include "SyntaxDefinition.h"
#include "ByteArray.h"

namespace flux
{

template<class SubClass> class Singleton;

class FormatSyntax: public SyntaxDefinition
{
public:
	bool find(const ByteArray *text, int *i0, int *i1, int *w, int *wi, int *wf, int *base, bool *exp, char *blank) const;

protected:
	friend class Singleton<FormatSyntax>;

	FormatSyntax();

	int width_;
	int integerWidth_;
	int fractionWidth_;
	int base_;
	int exp_;
	int blank_;
	int specifier_;
};

const FormatSyntax *formatSyntax();

} // namespace flux

#endif // FLUX_FORMATSYNTAX_H
