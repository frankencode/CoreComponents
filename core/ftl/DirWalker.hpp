 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */
#ifndef FTL_DIRWALKER_HPP
#define FTL_DIRWALKER_HPP

#include "string.hpp"

namespace ftl
{

class Dir;

class DirWalker: public Source<string>
{
public:
	enum Option {
		DirsFirst,
		FilesFirst,
		DirsOnly,
		FilesOnly
	};

	static hook<DirWalker> open(string path, int option = DirsFirst);
	inline int maxDepth() const { return maxDepth_; }
	inline void setMaxDepth(int depth) { maxDepth_ = depth; }

	bool read(string *path);

private:
	hook<DirWalker> tryOpen(string path, int option);
	DirWalker(string path, int option, Dir *dir = 0);
	int option_;
	int maxDepth_;
	int depth_;
	hook<Dir> dir_;
	hook<DirWalker> child_;
};

} // namespace ftl

#endif // FTL_DIRWALKER_HPP
