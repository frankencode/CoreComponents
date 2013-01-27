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
	inline static O<GccToolChain> create(String execPath = "g++") {
		return new GccToolChain(execPath);
	}

	virtual String machineCommand() const;
	virtual int defaultSpeedOptimizationLevel() const;
	virtual int defaultSizeOptimizationLevel() const;

	virtual String analyseCommand(BuildPlan *buildPlan, String source) const;
	virtual O<Job> createAnalyseJob(BuildPlan *buildPlan, String source);
	virtual O<Module> finishAnalyseJob(BuildPlan *buildPlan, Job *job);

	virtual O<Job> createCompileJob(BuildPlan *buildPlan, Module *module);
	virtual O<Job> createLinkJob(BuildPlan *buildPlan, Module *module);

	virtual String linkPath(BuildPlan *buildPlan) const;
	virtual bool link(BuildPlan *buildPlan);

	virtual void clean(BuildPlan *buildPlan);

protected:
	GccToolChain(String execPath);
	static String machineCommand(String execPath);

	static void appendCompileOptions(Format args, BuildPlan *buildPlan);
	static void appendLinkOptions(Format args, BuildPlan *buildPlan);

	static String lookup(String execPath);
};

} // namespace mach

#endif // MACH_GCCTOOLCHAIN_HPP
