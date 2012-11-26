#ifndef FTL_GCCTOOLCHAIN_HPP
#define FTL_GCCTOOLCHAIN_HPP

#include "ToolChain.hpp"
#include "Format.hpp"

namespace ftl
{

class GccToolChain: public ToolChain
{
public:
	inline static Ref<GccToolChain, Owner> create(String execPath = "g++") {
		return new GccToolChain(execPath);
	}

	virtual String analyseCommand(Ref<BuildPlan> buildPlan, String source) const;
	virtual Ref<Module, Owner> analyse(Ref<BuildPlan> buildPlan, String source);
	virtual bool compile(Ref<BuildPlan> buildPlan, Ref<Module, Owner> module);

	virtual String linkPath(Ref<BuildPlan> buildPlan) const;
	virtual bool link(Ref<BuildPlan> buildPlan);

	virtual void clean(Ref<BuildPlan> buildPlan);
	virtual void distClean(Ref<BuildPlan> buildPlan);

protected:
	GccToolChain(String execPath);
	void appendCompileOptions(Format args, int options, Ref<StringList> includePaths, String outputPath = "") const;

	static String lookup(String execPath);
};

} // namespace ftl

#endif // FTL_GCCTOOLCHAIN_HPP
