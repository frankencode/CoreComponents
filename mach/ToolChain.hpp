#ifndef FTL_TOOLCHAIN_HPP
#define FTL_TOOLCHAIN_HPP

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

	virtual String analyseCommand(Ref<BuildPlan> buildPlan, String source) const = 0;
	virtual Ref<Job, Owner> createAnalyseJob(Ref<BuildPlan> buildPlan, String source) = 0;
	virtual Ref<Module, Owner> finishAnalyseJob(Ref<BuildPlan> buildPlan, Ref<Job> job) = 0;

	virtual bool compile(Ref<BuildPlan> buildPlan, Ref<Module> module) = 0;

	virtual String linkPath(Ref<BuildPlan> buildPlan) const = 0;
	virtual bool link(Ref<BuildPlan> buildPlan) = 0;

	virtual void clean(Ref<BuildPlan> buildPlan) = 0;

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

#endif // FTL_TOOLCHAIN_HPP
