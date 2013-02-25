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
	static Ref<DirWalker> open(String path);

	inline int maxDepth() const { return maxDepth_; }
	inline void setMaxDepth(int depth) { maxDepth_ = depth; }

	inline bool followSymlink() const { return followSymlink_; }
	inline void setFollowSymlink(bool on) { followSymlink_ = on; }

	inline bool deleteOrder() const { return deleteOrder_; }
	inline void setDeleteOrder(bool on) { deleteOrder_ = on; }

	bool read(String *path);

private:
	Ref<DirWalker> tryOpen(String path);
	DirWalker(String path, Dir *dir = 0);
	int maxDepth_;
	bool followSymlink_;
	bool deleteOrder_;
	int depth_;
	Ref<Dir> dir_;
	Ref<DirWalker> child_;
};

} // namespace ftl

#endif // FTL_DIRWALKER_HPP
