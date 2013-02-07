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

#include "String.hpp"

namespace ftl
{

class Dir;

class DirWalker: public Source<String>
{
public:
	enum Option {
		DirsFirst,
		FilesFirst,
		DirsOnly,
		FilesOnly
	};

	static Ref<DirWalker> open(String path, int option = DirsFirst);
	inline int maxDepth() const { return maxDepth_; }
	inline void setMaxDepth(int depth) { maxDepth_ = depth; }

	bool read(String *path);

private:
	Ref<DirWalker> tryOpen(String path, int option);
	DirWalker(String path, int option, Dir *dir = 0);
	int option_;
	int maxDepth_;
	int depth_;
	Ref<Dir> dir_;
	Ref<DirWalker> child_;
};

} // namespace ftl

#endif // FTL_DIRWALKER_HPP
