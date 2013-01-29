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
	inline static hook<GccToolChain> create(string execPath = "g++") {
		return new GccToolChain(execPath);
	}

	virtual string machineCommand() const;
	virtual int defaultSpeedOptimizationLevel() const;
	virtual int defaultSizeOptimizationLevel() const;

	virtual string analyseCommand(BuildPlan *buildPlan, string source) const;
	virtual hook<Job> createAnalyseJob(BuildPlan *buildPlan, string source);
	virtual hook<Module> finishAnalyseJob(BuildPlan *buildPlan, Job *job);

	virtual hook<Job> createCompileJob(BuildPlan *buildPlan, Module *module);
	virtual hook<Job> createLinkJob(BuildPlan *buildPlan, Module *module);

	virtual string linkPath(BuildPlan *buildPlan) const;
	virtual bool link(BuildPlan *buildPlan);

	virtual void clean(BuildPlan *buildPlan);

protected:
	GccToolChain(string execPath);
	static string machineCommand(string execPath);

	static void appendCompileOptions(Format args, BuildPlan *buildPlan);
	static void appendLinkOptions(Format args, BuildPlan *buildPlan);

	static string lookup(string execPath);
};

} // namespace mach

#endif // MACH_GCCTOOLCHAIN_HPP
