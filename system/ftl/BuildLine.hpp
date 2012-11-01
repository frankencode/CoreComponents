#ifndef FTL_BUILDLINE_HPP
#define FTL_BUILDLINE_HPP

#include "String.hpp"

namespace ftl
{

class FileStatus;

class BuildLine: public Instance
{
public:
	virtual String runAnalyse(String command) = 0;
	virtual bool runBuild(String command) = 0;
	virtual bool symlink(String path, String newPath) = 0;
	virtual bool unlink(String path) = 0;
	virtual Ref<FileStatus, Owner> fileStatus(String path) = 0;
};

} // namespace ftl

#endif // FTL_BUILDLINE_HPP
