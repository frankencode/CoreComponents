#include "Dir.hpp"
#include "FileStatus.hpp"
#include "DirWalker.hpp"

namespace ftl
{

Ref<DirWalker, Owner> DirWalker::tryOpen(String path, int option)
{
	Ref<Dir, Owner> dir = Dir::tryOpen(path);
	if (dir) return new DirWalker(path, option, dir);
	return 0;
}

Ref<DirWalker, Owner> DirWalker::open(String path, int option)
{
	return new DirWalker(path, option);
}

DirWalker::DirWalker(String path, int option, Ref<Dir> dir)
	: option_(option),
	  maxDepth_(-1),
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
		--depth_;
		if ((option_ != FilesOnly) && (option_ != DirsFirst)) {
			*path = child_->dir_->path();
			child_ = 0;
			return true;
		}
		child_ = 0;
	}
	String name;
	while (dir_->read(&name)) {
		if ((name == "") || (name == ".") || (name == "..")) continue;
		String h = dir_->path(name);
		child_ = tryOpen(h, option_);
		if (child_) {
			if (depth_ == maxDepth_) continue;
			++depth_;
			if ((option_ != FilesOnly) && (option_ != DirsFirst))
				return read(path);
		}
		*path = h;
		return true;
	}
	return false;
}

} // namespace ftl
