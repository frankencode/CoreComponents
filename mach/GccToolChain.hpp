#ifndef FTL_GCCTOOLCHAIN_HPP
#define FTL_GCCTOOLCHAIN_HPP

#include "ToolChain.hpp"
#include <ftl/Format.hpp>

namespace ftl
{

class GccToolChain: public ToolChain
{
public:
	inline static Ref<GccToolChain, Owner> create(String execPath = "g++") {
		return new GccToolChain(execPath);
	}

	virtual String machineCommand() const;
	virtual int defaultSpeedOptimizationLevel() const;
	virtual int defaultSizeOptimizationLevel() const;

	virtual String analyseCommand(Ref<BuildPlan> buildPlan, String source) const;
	virtual Ref<Module, Owner> analyse(Ref<BuildPlan> buildPlan, String source);
	virtual bool compile(Ref<BuildPlan> buildPlan, Ref<Module, Owner> module);

	virtual String linkPath(Ref<BuildPlan> buildPlan) const;
	virtual bool link(Ref<BuildPlan> buildPlan);

	virtual void clean(Ref<BuildPlan> buildPlan);

protected:
	GccToolChain(String execPath);
	static String machineCommand(String execPath);

	bool linkTool(Ref<BuildPlan> buildPlan, Ref<Module, Owner> module);
	void appendCompileOptions(Format args, Ref<BuildPlan> buildPlan, String outputPath = "") const;
	void appendLinkOptions(Format args, Ref<StringList> libraryPaths, Ref<StringList> libraries) const;

	static String lookup(String execPath);
};

} // namespace ftl

#endif // FTL_GCCTOOLCHAIN_HPP
