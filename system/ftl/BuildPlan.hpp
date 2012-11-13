#ifndef FTL_BUILDPLAN_HPP
#define FTL_BUILDPLAN_HPP

#include "Config.hpp"
#include "Module.hpp"
#include "BuildLine.hpp"

namespace ftl
{

class FileStatus;
class ToolChain;
class BuildPlan;

typedef List< Ref<BuildPlan, Owner> > BuildPlanList;

class BuildPlan: public BuildLine
{
public:
	enum Option {
		Static        = 1,
		Library       = 2,
		Debug         = 4,
		Release       = 8,
		OptimizeSpeed = 16,
		OptimizeSize  = 32,
		DryRun        = 64,
		Blindfold     = 128,
		Verbose       = 256,
		Unspecified   = -1,
		GlobalOptions = Debug|Release|OptimizeSpeed|OptimizeSize|DryRun|Blindfold|Verbose
	};

	static Ref<BuildPlan, Owner> create(String projectPath = ".", int globalOptions = Unspecified);

	inline String projectPath() const { return projectPath_; }
	inline Ref<Config> recipe() const { return recipe_; }

	inline String name() const { return name_; }
	inline String version() const { return version_; }
	inline int options() const { return options_; }

	inline Ref<StringList> includePaths() const { return includePaths_; }
	inline Ref<StringList> libraryPaths() const { return libraryPaths_; }
	inline Ref<StringList> libraries() const { return libraries_; }
	inline Ref<StringList> sources() const { return sources_; }
	inline Ref<ModuleList> modules() const { return modules_; }

	inline Ref<BuildPlanList> prequisites() const { return prequisites_; }

	int run(Ref<ToolChain> toolChain, int argc, char **argv);

private:
	BuildPlan(String projectPath, int globalOptions = Unspecified);

	virtual String runAnalyse(String command);
	virtual bool runBuild(String command);
	virtual bool symlink(String path, String newPath);
	virtual bool unlink(String path);
	virtual Ref<FileStatus, Owner> fileStatus(String path);

	void analyse(Ref<ToolChain> toolChain);
	bool build(Ref<ToolChain> toolChain);
	void clean(Ref<ToolChain> toolChain);
	void distClean(Ref<ToolChain> toolChain);

	static int optionsFromRecipe(Ref<Config> recipe);

	String projectPath_;
	Ref<Config, Owner> recipe_;

	String name_;
	String version_;
	int options_;

	Ref<StringList, Owner> includePaths_;
	Ref<StringList, Owner> libraryPaths_;
	Ref<StringList, Owner> libraries_;
	Ref<StringList, Owner> sources_;
	Ref<ModuleList, Owner> modules_;

	Ref<BuildPlanList, Owner> prequisites_;
};

} // namespace ftl

#endif // FTL_BUILDPLAN_HPP
