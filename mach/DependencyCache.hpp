#ifndef MACH_DEPENDENCYCACHE_HPP
#define MACH_DEPENDENCYCACHE_HPP

#include <ftl/String.hpp>
#include <ftl/Time.hpp>

namespace ftl {
class File;
class WireObject;
} // namespace ftl

namespace mach
{

using namespace ftl;

class Module;
class BuildPlan;

class DependencyCache: public Instance
{
public:
	static Ref<DependencyCache> create(BuildPlan *buildPlan);
	~DependencyCache();

	static String cachePath(BuildPlan *buildPlan);

	bool lookup(String source, Ref<Module> *module);
	void insert(String source, Module *module);

private:
	DependencyCache(BuildPlan *buildPlan);

	Ref<BuildPlan> buildPlan_;
	String cachePath_;
	typedef Map< String, Ref<Module> > Cache;
	Ref<Cache> cache_;
};

} // namespace mach

#endif // MACH_DEPENDENCYCACHE_HPP
