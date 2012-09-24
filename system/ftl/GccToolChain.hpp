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

	virtual String analyseCommand(String source, int options = 0) const;
	virtual Ref<Module, Owner> analyse(String source, int options = 0);
	virtual bool compile(Ref<Module, Owner> modules, int options = 0);
	virtual String linkPath(String name, String version, int options) const;
	virtual bool link(Ref<ModuleList> modules, Ref<StringList> libraries, String name, String version, int options = 0);

protected:
	GccToolChain(String execPath);
	void appendCompileOptions(Format args, int options, String modulePath = "") const;

	static String lookup(String execPath);
};

} // namespace ftl

#endif // FTL_GCCTOOLCHAIN_HPP
