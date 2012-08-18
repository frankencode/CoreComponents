/*
 * Glob.hpp -- recursive directory listings
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */
#ifndef FTL_GLOB_HPP
#define FTL_GLOB_HPP

#include "DirEntry.hpp"
#include "Pattern.hpp"

namespace ftl
{

class Dir;

class Glob: public Source<DirEntry>
{
public:
	inline static Ref<Glob, Owner> newInstance(String expression) { return new Glob(expression); }

	bool read(DirEntry *entry);

private:
	Glob(String expression);
	Glob(String path, Ref<StringList> remainder);
	void init(String path);

	Ref<Dir, Owner> dir_;
	Pattern pattern_;
	Ref<StringList, Owner> remainder_;
	Ref<Glob, Owner> child_;
};

} // namespace ftl

#endif // FTL_GLOB_HPP
