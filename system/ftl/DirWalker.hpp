#ifndef FTL_DIRWALKER_HPP
#define FTL_DIRWALKER_HPP

#include "String.hpp"

namespace ftl
{

class Dir;

class DirWalker: public Source<String>
{
public:
	inline Ref<DirWalker, Owner> open(String path) { return new DirWalker(path); }
	bool read(String *path);

private:
	Ref<DirWalker, Owner> tryOpen(String path);
	DirWalker(String path, Ref<Dir> dir = 0);
	Ref<Dir, Owner> dir_;
	Ref<DirWalker, Owner> child_;
};

} // namespace ftl

#endif // FTL_DIRWALKER_HPP
