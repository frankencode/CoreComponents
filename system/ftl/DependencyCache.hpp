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
	static Ref<DependencyCache, Owner> newInstance(Ref<ToolChain> compiler, Ref<StringList> sources, int options, Ref<StringList> includePaths, String cachePath = "DependencyCache");
	~DependencyCache();

	Ref<Module, Owner> analyse(String sources, int options, Ref<StringList> includePaths);

private:
	DependencyCache(Ref<ToolChain> compiler, Ref<StringList> sourcePaths, int options, Ref<StringList> includePaths, String cachePath);

	Ref<ToolChain, Owner> compiler_;
	Ref<File, Owner> cacheFile_;
	typedef Map< String, Ref<Module, Owner> > Cache;
	Ref<Cache, Owner> cache_;
};

} // namespace ftl

#endif // FTL_DEPENDENCYCACHE_HPP
