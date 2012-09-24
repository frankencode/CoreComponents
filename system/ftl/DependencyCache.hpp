#ifndef FTL_DEPENDENCYCACHE_HPP
#define FTL_DEPENDENCYCACHE_HPP

#include "String.hpp"
#include "Time.hpp"

namespace ftl
{

class ToolChain;
class Module;
class File;
class WireObject;

class DependencyCache: public Instance
{
public:
	inline static Ref<DependencyCache, Owner> newInstance(Ref<ToolChain> compiler, Ref<StringList> sourcePaths, int options, String cachePath = "DependencyCache") {
		return new DependencyCache(compiler, sourcePaths, options, cachePath);
	}

	~DependencyCache();

	Ref<Module, Owner> analyse(String sourcePath, int options);

private:
	DependencyCache(Ref<ToolChain> compiler, Ref<StringList> sourcePaths, int options, String cachePath);

	Ref<ToolChain, Owner> compiler_;
	Ref<File, Owner> cacheFile_;
	typedef Map< String, Ref<Module, Owner> > Cache;
	Ref<Cache, Owner> cache_;
};

} // namespace ftl

#endif // FTL_DEPENDENCYCACHE_HPP
