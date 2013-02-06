#ifndef MACH_GCCTOOLCHAIN_HPP
#define MACH_GCCTOOLCHAIN_HPP

#include <ftl/format.hpp>
#include "ToolChain.hpp"

namespace mach
{

using namespace ftl;

class GccToolChain: public ToolChain
{
public:
	inline static Ref<GccToolChain> create(string execPath = "g++") {
		return new GccToolChain(execPath);
	}

	virtual string machineCommand() const;
	virtual int defaultSpeedOptimizationLevel() const;
	virtual int defaultSizeOptimizationLevel() const;

	virtual string analyseCommand(BuildPlan *buildPlan, string source) const;
	virtual Ref<Job> createAnalyseJob(BuildPlan *buildPlan, string source);
	virtual Ref<Module> finishAnalyseJob(BuildPlan *buildPlan, Job *job);

	virtual Ref<Job> createCompileJob(BuildPlan *buildPlan, Module *module);
	virtual Ref<Job> createLinkJob(BuildPlan *buildPlan, Module *module);

	virtual string linkPath(BuildPlan *buildPlan) const;
	virtual bool link(BuildPlan *buildPlan);

	virtual void clean(BuildPlan *buildPlan);

protected:
	GccToolChain(string execPath);
	static string machineCommand(string execPath);

	static void appendCompileOptions(format args, BuildPlan *buildPlan);
	static void appendLinkOptions(format args, BuildPlan *buildPlan);

	static string lookup(string execPath);
};

} // namespace mach

#endif // MACH_GCCTOOLCHAIN_HPP
