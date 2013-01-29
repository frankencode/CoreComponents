 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */
#ifndef FTL_GLOB_HPP
#define FTL_GLOB_HPP

#include "pattern.hpp"

namespace ftl
{

class Dir;

class Glob: public Source<string>
{
public:
	inline static hook<Glob> open(string expression) { return new Glob(expression); }

	bool read(string *path);

private:
	Glob(string expression);
	Glob(string path, StringList *remainder);
	void init(string path);

	hook<Dir> dir_;
	pattern pattern_;
	hook<StringList> remainder_;
	hook<Glob> child_;
};

} // namespace ftl

#endif // FTL_GLOB_HPP
