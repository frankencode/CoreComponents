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
	inline String execPath() const { return execPath_; }
	inline String machine() const { return machine_; }

	virtual String machineCommand() const = 0;
	virtual int defaultSpeedOptimizationLevel() const = 0;
	virtual int defaultSizeOptimizationLevel() const = 0;

	virtual String analyseCommand(BuildPlan *buildPlan, String source) const = 0;
	virtual Ref<Job, Owner> createAnalyseJob(BuildPlan *buildPlan, String source) = 0;
	virtual Ref<Module, Owner> finishAnalyseJob(BuildPlan *buildPlan, Job *job) = 0;

	virtual Ref<Job, Owner> createCompileJob(BuildPlan *buildPlan, Module *module) = 0;
	virtual Ref<Job, Owner> createLinkJob(BuildPlan *buildPlan, Module *module) = 0;

	virtual String linkPath(BuildPlan *buildPlan) const = 0;
	virtual bool link(BuildPlan *buildPlan) = 0;

	virtual void clean(BuildPlan *buildPlan) = 0;

protected:
	ToolChain(String execPath, String machine)
		: execPath_(execPath),
		  machine_(machine)
	{}

private:
	String execPath_;
	String machine_;
};

} // namespace mach

#endif // MACH_TOOLCHAIN_HPP
