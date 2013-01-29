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

#include "Pattern.hpp"

namespace ftl
{

class Dir;

class Glob: public Source<String>
{
public:
	inline static hook<Glob> open(String expression) { return new Glob(expression); }

	bool read(String *path);

private:
	Glob(String expression);
	Glob(String path, StringList *remainder);
	void init(String path);

	hook<Dir> dir_;
	Pattern pattern_;
	hook<StringList> remainder_;
	hook<Glob> child_;
};

} // namespace ftl

#endif // FTL_GLOB_HPP
