#ifndef MACH_GCCTOOLCHAIN_HPP
#define MACH_GCCTOOLCHAIN_HPP

#include <ftl/Format.hpp>
#include "ToolChain.hpp"

namespace mach
{

using namespace ftl;

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
	virtual Ref<Job, Owner> createAnalyseJob(Ref<BuildPlan> buildPlan, String source);
	virtual Ref<Module, Owner> finishAnalyseJob(Ref<BuildPlan> buildPlan, Ref<Job> job);

	virtual Ref<Job, Owner> createCompileJob(Ref<BuildPlan> buildPlan, Ref<Module> module);

	virtual String linkPath(Ref<BuildPlan> buildPlan) const;
	virtual bool link(Ref<BuildPlan> buildPlan);

	virtual void clean(Ref<BuildPlan> buildPlan);

protected:
	GccToolChain(String execPath);
	static String machineCommand(String execPath);

	void appendCompileOptions(Format args, Ref<BuildPlan> buildPlan, String outputPath = "") const;
	void appendLinkOptions(Format args, Ref<StringList> libraryPaths, Ref<StringList> libraries) const;

	static String lookup(String execPath);
};

} // namespace mach

#endif // MACH_GCCTOOLCHAIN_HPP
