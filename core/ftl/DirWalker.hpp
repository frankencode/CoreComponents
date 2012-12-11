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
		FilesOnly,
		DirsOnly
	};

	inline Ref<DirWalker, Owner> open(String path, int option = 0) { return new DirWalker(path, option); }
	bool read(String *path);

private:
	Ref<DirWalker, Owner> tryOpen(String path, int option = 0);
	DirWalker(String path, int option, Ref<Dir> dir = 0);
	int option_;
	Ref<Dir, Owner> dir_;
	Ref<DirWalker, Owner> child_;
};

} // namespace ftl

#endif // FTL_DIRWALKER_HPP
