#ifndef FTL_DEPENDENCYCACHE_HPP
#define FTL_DEPENDENCYCACHE_HPP

#include <ftl/String.hpp>
#include <ftl/Time.hpp>

namespace ftl {
	class File;
	class WireObject;
} // namespace ftl

namespace mach
{

using namespace ftl;

class ToolChain;
class Module;
class BuildPlan;

class DependencyCache: public Instance
{
public:
	static Ref<DependencyCache, Owner> create(Ref<BuildPlan> buildPlan);
	~DependencyCache();

	static String cachePath(Ref<BuildPlan> buildPlan);

	Ref<Module, Owner> analyse(String source);

private:
	DependencyCache(Ref<BuildPlan> buildPlan);

	Ref<BuildPlan, Owner> buildPlan_;
	String cachePath_;
	typedef Map< String, Ref<Module, Owner> > Cache;
	Ref<Cache, Owner> cache_;
};

} // namespace mach

#endif // FTL_DEPENDENCYCACHE_HPP
