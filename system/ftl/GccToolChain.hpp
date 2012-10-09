#ifndef FTL_GCCTOOLCHAIN_HPP
#define FTL_GCCTOOLCHAIN_HPP

#include "ToolChain.hpp"
#include "Format.hpp"

namespace ftl
{

class GccToolChain: public ToolChain
{
public:
	inline static Ref<GccToolChain, Owner> newInstance(String execPath = "g++") {
		return new GccToolChain(execPath);
	}

	virtual String analyseCommand(String source, int options, Ref<StringList> includePaths) const;
	virtual Ref<Module, Owner> analyse(String source, int options, Ref<StringList> includePaths);

	virtual bool compile(Ref<Module, Owner> modules, int options, Ref<StringList> includePaths);

	virtual String linkPath(String name, String version, int options) const;

	virtual bool link(
		Ref<ModuleList> modules,
		Ref<StringList> libraryPaths,
		Ref<StringList> libraries,
		String name,
		String version,
		int options = 0
	);

	virtual void clean(Ref<ModuleList> modules, int options);
	virtual void distClean(Ref<ModuleList> modules, String name, String version, int options);

protected:
	GccToolChain(String execPath);
	void appendCompileOptions(Format args, int options, Ref<StringList> includePaths, String modulePath = "") const;

	static String lookup(String execPath);
};

} // namespace ftl

#endif // FTL_GCCTOOLCHAIN_HPP
