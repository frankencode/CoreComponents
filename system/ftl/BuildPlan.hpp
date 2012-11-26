#ifndef FTL_BUILDPLAN_HPP
#define FTL_BUILDPLAN_HPP

#include "Module.hpp"
#include "ToolChain.hpp"

namespace ftl
{

class FileStatus;
class Config;
class BuildPlan;

typedef List< Ref<BuildPlan, Owner> > BuildPlanList;

class BuildPlan: public Instance
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

	static Ref<BuildPlan, Owner> create(int argc, char **argv);

	inline Ref<ToolChain> toolChain() const { return toolChain_; }
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

	int run();

	String runAnalyse(String command);
	bool runBuild(String command);
	bool symlink(String path, String newPath);
	bool unlink(String path);
	Ref<FileStatus, Owner> fileStatus(String path);

protected:
	static Ref<BuildPlan, Owner> create(Ref<ToolChain> toolChain, String projectPath, int globalOptions);

	BuildPlan(int argc, char **argv);
	BuildPlan(Ref<ToolChain> toolChain, String projectPath, int globalOptions = Unspecified);

	void readRecipe();
	String sourcePath(String source) const;

	void analyse();
	bool build();
	void clean();
	void distClean();

private:
	Ref<ToolChain, Owner> toolChain_;
	String projectPath_;
	String buildPath_;
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
