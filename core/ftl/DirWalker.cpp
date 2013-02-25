 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */

#include "Dir.hpp"
#include "File.hpp"
#include "DirWalker.hpp"

namespace ftl
{

Ref<DirWalker> DirWalker::tryOpen(String path)
{
	Ref<Dir> dir = Dir::tryOpen(path);
	if (dir) return new DirWalker(path, dir);
	return 0;
}

Ref<DirWalker> DirWalker::open(String path)
{
	return new DirWalker(path);
}

DirWalker::DirWalker(String path, Dir *dir)
	: maxDepth_(-1),
	  followSymlink_(false),
	  deleteOrder_(false),
	  depth_(0),
	  dir_(dir)
{
	if (!dir_) dir_ = Dir::open(path);
}

bool DirWalker::read(String *path)
{
	if (child_) {
		if (child_->read(path))
			return true;
		if (deleteOrder_) {
			*path = child_->dir_->path();
			child_ = 0;
			return true;
		}
		child_ = 0;
	}
	String name;
	while (dir_->read(&name)) {
		if (name == "" || name == "." || name == "..") continue;
		String h = dir_->path(name);
		child_ = tryOpen(h);
		if (child_) {
			if (depth_ != maxDepth_) {
				if (!followSymlink_ && File::readlink(h) != "") {
					child_ = 0;
				}
				else {
					child_->maxDepth_ = maxDepth_;
					child_->followSymlink_ = followSymlink_;
					child_->deleteOrder_ = deleteOrder_;
					child_->depth_ = depth_ + 1;
					if (deleteOrder_)
						return read(path);
				}
			}
			else {
				child_ = 0;
			}
		}
		*path = h;
		return true;
	}
	return false;
}

} // namespace ftl
