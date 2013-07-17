/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FKIT_FORMATSPECIFIER_H
#define FKIT_FORMATSPECIFIER_H

#include "SyntaxDefinition.h"
#include "Singleton.h"
#include "ByteArray.h"

namespace fkit
{

class FormatSpecifier: public SyntaxDefinition, public Singleton<FormatSpecifier>
{
public:
	bool find(ByteArray *text, int *i0, int *i1, int *w, int *wi, int *wf, int *base, bool *exp, char *blank);

protected:
	friend class Singleton<FormatSpecifier>;

	FormatSpecifier();

	int width_;
	int integerWidth_;
	int fractionWidth_;
	int base_;
	int exp_;
	int blank_;
	int format_;
};

inline FormatSpecifier *formatSpecifier() { return FormatSpecifier::instance(); }

} // namespace fkit

#endif // FKIT_FORMATSPECIFIER_H
