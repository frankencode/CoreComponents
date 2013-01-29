#ifndef MACH_TOOLCHAIN_HPP
#define MACH_TOOLCHAIN_HPP

#include <ftl/Config.hpp>
#include "Job.hpp"
#include "Module.hpp"

namespace mach
{

using namespace ftl;

FTL_STD_EXCEPTION(MachException);

class BuildPlan;
class DependencyCache;

class ToolChain: public Instance
{
public:
	inline string execPath() const { return execPath_; }
	inline string machine() const { return machine_; }

	virtual string machineCommand() const = 0;
	virtual int defaultSpeedOptimizationLevel() const = 0;
	virtual int defaultSizeOptimizationLevel() const = 0;

	virtual string analyseCommand(BuildPlan *buildPlan, string source) const = 0;
	virtual hook<Job> createAnalyseJob(BuildPlan *buildPlan, string source) = 0;
	virtual hook<Module> finishAnalyseJob(BuildPlan *buildPlan, Job *job) = 0;

	virtual hook<Job> createCompileJob(BuildPlan *buildPlan, Module *module) = 0;
	virtual hook<Job> createLinkJob(BuildPlan *buildPlan, Module *module) = 0;

	virtual string linkPath(BuildPlan *buildPlan) const = 0;
	virtual bool link(BuildPlan *buildPlan) = 0;

	virtual void clean(BuildPlan *buildPlan) = 0;

protected:
	ToolChain(string execPath, string machine)
		: execPath_(execPath),
		  machine_(machine)
	{}

private:
	string execPath_;
	string machine_;
};

} // namespace mach

#endif // MACH_TOOLCHAIN_HPP
