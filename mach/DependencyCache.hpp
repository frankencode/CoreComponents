#ifndef MACH_DEPENDENCYCACHE_HPP
#define MACH_DEPENDENCYCACHE_HPP

#include <ftl/string.hpp>
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
	static hook<DependencyCache> create(BuildPlan *buildPlan);
	~DependencyCache();

	static string cachePath(BuildPlan *buildPlan);

	bool lookup(string source, hook<Module> *module);
	void insert(string source, Module *module);

private:
	DependencyCache(BuildPlan *buildPlan);

	hook<BuildPlan> buildPlan_;
	string cachePath_;
	typedef Map< string, hook<Module> > Cache;
	hook<Cache> cache_;
};

} // namespace mach

#endif // MACH_DEPENDENCYCACHE_HPP
