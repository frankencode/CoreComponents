#include "Dir.hpp"
#include "FileStatus.hpp"
#include "DirWalker.hpp"

namespace ftl
{

Ref<DirWalker, Owner> DirWalker::tryOpen(String path)
{
	Ref<Dir, Owner> dir = Dir::tryOpen(path);
	if (dir) return new DirWalker(path, dir);
	return 0;
}

DirWalker::DirWalker(String path, Ref<Dir> dir)
	: dir_(dir)
{
	if (!dir_) dir_ = Dir::open(path);
}

bool DirWalker::read(String *path)
{
	if (child_) {
		if (child_->read(path))
			return true;
		*path = child_->dir_->path();
		child_ = 0;
		return true;
	}
	String name;
	if (dir_->read(&name)) {
		if ((name == ".") || (name == ".."))
			return read(path);
		String h = dir_->path(name);
		child_ = tryOpen(h);
		if (child_) return read(path);
		*path = h;
		return true;
	}
	return false;
}

} // namespace ftl
