/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_FORMATSYNTAX_H
#define FLUX_FORMATSYNTAX_H

#include <flux/SyntaxDefinition>
#include <flux/ByteArray>

namespace flux {

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
