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

	static Ref<DirWalker, Owner> open(String path, int option = DirsFirst);
	inline int maxDepth() const { return maxDepth_; }
	inline void setMaxDepth(int depth) { maxDepth_ = depth; }

	bool read(String *path);

private:
	Ref<DirWalker, Owner> tryOpen(String path, int option);
	DirWalker(String path, int option, Ref<Dir> dir = 0);
	int option_;
	int maxDepth_;
	int depth_;
	Ref<Dir, Owner> dir_;
	Ref<DirWalker, Owner> child_;
};

} // namespace ftl

#endif // FTL_DIRWALKER_HPP
