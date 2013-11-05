/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
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
