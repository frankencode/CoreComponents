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

DirWalker::DirWalker(String path, int option, Ref<Dir> dir)
	: option_(option),
	  dir_(dir)
{
	if (!dir_) dir_ = Dir::open(path);
}

bool DirWalker::read(String *path)
{
	if (child_) {
		if (child_->read(path))
			return true;
		child_ = 0;
		if ((option_ != FilesOnly) && (option_ != DirsFirst)) {
			*path = child_->dir_->path();
			return true;
		}
	}
	String name;
	while (dir_->read(&name)) {
		if ((name == "") || (name == ".") || (name == "..")) continue;
		String h = dir_->path(name);
		child_ = tryOpen(h, option_);
		if (child_) {
			if ((option_ != FilesOnly) && (option_ != DirsFirst))
				return read(path);
		}
		*path = h;
		return true;
	}
	return false;
}

} // namespace ftl
