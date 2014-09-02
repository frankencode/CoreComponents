/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_GLOB_H
#define FLUX_GLOB_H

#include "Pattern.h"

namespace flux
{

class Dir;

class Glob: public Source<String>
{
public:
	inline static Ref<Glob> open(String expression) { return new Glob(expression); }

	bool read(String *path);

private:
	Glob(String expression);
	Glob(String path, StringList *remainder);
	void init(String path);

	Ref<Dir> dir_;
	Pattern pattern_;
	Ref<StringList> remainder_;
	Ref<Glob> child_;
};

} // namespace flux

#endif // FLUX_GLOB_H
