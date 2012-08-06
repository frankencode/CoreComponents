#ifndef FTL_MACHDEPENDENCYCACHE_HPP
#define FTL_MACHDEPENDENCYCACHE_HPP

#include "String.hpp"
#include "Time.hpp"

namespace ftl
{

class MachCompiler;
class MachObject;
class File;
class WireObject;

class MachDependencyCache: public Instance
{
public:
	MachDependencyCache(Ref<MachCompiler> compiler, Ref<StringList> sourcePaths, String cachePath = "DependencyCache");
	~MachDependencyCache();

	Ref<MachObject, Owner> analyse(String sourcePath);

private:
	Ref<MachCompiler, Owner> compiler_;
	Ref<File, Owner> cacheFile_;
	Time cacheTime_;
	typedef Map< String, Ref<MachObject> > Cache;
	Ref<Cache, Owner> cache_;
};

} // namespace ftl

#endif // FTL_MACHDEPENDENCYCACHE_HPP
