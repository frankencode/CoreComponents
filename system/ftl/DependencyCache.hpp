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
class BuildLine;

class DependencyCache: public Instance
{
public:
	static Ref<DependencyCache, Owner> create(Ref<BuildLine> buildLine, Ref<ToolChain> toolChain, Ref<StringList> sources, int options, Ref<StringList> includePaths, String cachePath = "DependencyCache");
	~DependencyCache();

	Ref<Module, Owner> analyse(String sources, int options, Ref<StringList> includePaths);

private:
	DependencyCache(Ref<BuildLine> buildLine, Ref<ToolChain> toolChain, Ref<StringList> sourcePaths, int options, Ref<StringList> includePaths, String cachePath);

	Ref<BuildLine, Owner> buildLine_;
	Ref<ToolChain, Owner> toolChain_;
	String cachePath_;
	typedef Map< String, Ref<Module, Owner> > Cache;
	Ref<Cache, Owner> cache_;
};

} // namespace ftl

#endif // FTL_DEPENDENCYCACHE_HPP
