#ifndef FTL_TOOLCHAIN_HPP
#define FTL_TOOLCHAIN_HPP

#include "Config.hpp"
#include "Module.hpp"
#include "BuildPlan.hpp"

namespace ftl
{

FTL_STD_EXCEPTION(MachException);

class DependencyCache;

class ToolChain: public Instance
{
public:
	inline String execPath() const { return execPath_; }
	inline String machine() const { return machine_; }

	virtual String analyseCommand(String source, int options, Ref<StringList> includePaths) const = 0;
	virtual Ref<Module, Owner> analyse(String source, int options, Ref<StringList> includePaths) = 0;

	virtual bool compile(Ref<Module, Owner> module, int options, Ref<StringList> includePaths) = 0;

	virtual String linkPath(String name, String version, int options) const = 0;

	virtual bool link(
		Ref<ModuleList> modules,
		Ref<StringList> libraryPaths,
		Ref<StringList> libraries,
		String name,
		String version,
		int options = 0
	) = 0;

	virtual void clean(Ref<ModuleList> modules, int options) = 0;
	virtual void distClean(Ref<ModuleList> modules, String name, String version, int options) = 0;
	bool unlink(const char *path);

protected:
	ToolChain();
	void init(String execPath, String machine);

private:
	String execPath_;
	String machine_;
};

} // namespace ftl

#endif // FTL_TOOLCHAIN_HPP
