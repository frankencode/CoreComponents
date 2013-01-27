#ifndef MACH_BUILDPLAN_HPP
#define MACH_BUILDPLAN_HPP

#include <ftl/PrefixTree.hpp>
#include "Module.hpp"
#include "ToolChain.hpp"

namespace ftl {
class FileStatus;
class Config;
} // namespace ftl

namespace mach
{

using namespace ftl;

class BuildPlan;

typedef List< O<BuildPlan> > BuildPlanList;

FTL_EXCEPTION(BuildPlanException, Exception);

class BuildPlan: public Instance
{
public:
	enum Option {
		Static        = 1,
		Application   = 2,
		Library       = 4,
		ToolSet       = 8,
		Package       = 16,
		Debug         = 32,
		Release       = 64,
		DryRun        = 128,
		Blindfold     = 256,
		Bootstrap     = 512,
		Verbose       = 1024,
		OptimizeSpeed = 2048,
		OptimizeSize  = 4096,
		Unspecified   = -1,
		GlobalOptions = Debug|Release|DryRun|Blindfold|Bootstrap|Verbose|OptimizeSpeed|OptimizeSize
	};

	static O<BuildPlan> create(int argc, char **argv);

	inline ToolChain *toolChain() const { return toolChain_; }
	inline String projectPath() const { return projectPath_; }
	inline String modulePath() const { return modulePath_; }
	inline Config *recipe() const { return recipe_; }

	inline String name() const { return name_; }
	inline String version() const { return version_; }
	inline int options() const { return options_; }
	inline int speedOptimizationLevel() const { return speedOptimizationLevel_; }
	inline int sizeOptimizationLevel() const { return sizeOptimizationLevel_; }

	inline StringList *includePaths() const { return includePaths_; }
	inline StringList *libraryPaths() const { return libraryPaths_; }
	inline StringList *libraries() const { return libraries_; }
	inline StringList *sources() const { return sources_; }
	inline ModuleList *modules() const { return modules_; }

	inline BuildPlanList *prequisites() const { return prequisites_; }

	int run();

	String sourcePath(String source) const;
	String modulePath(String object) const;

	String beautifyCommand(String command);
	bool runBuild(String command);

	bool mkdir(String path);
	bool rmdir(String path);
	bool symlink(String path, String newPath);
	bool unlink(String path);
	O<FileStatus> fileStatus(String path);

private:
	O<BuildPlan> create(ToolChain *toolChain, String projectPath, int globalOptions);

	typedef PrefixTree<char, BuildPlan *> BuildMap;

	BuildPlan(int argc, char **argv);
	BuildPlan(ToolChain *toolChain, String projectPath, BuildPlan *parentPlan);

	void readRecipe(BuildPlan *parentPlan = 0);

	void prepare();
	bool analyse();
	bool build();
	void clean();

	O<ToolChain> toolChain_;
	String projectPath_;
	String modulePath_;
	O<Config> recipe_;

	String name_;
	String version_;
	int options_;
	int speedOptimizationLevel_;
	int sizeOptimizationLevel_;

	O<StringList> includePaths_;
	O<StringList> libraryPaths_;
	O<StringList> libraries_;
	O<StringList> sources_;
	O<ModuleList> modules_;

	O<BuildPlanList> prequisites_;

	O<BuildMap> buildMap_;
	String sourcePrefix_;

	bool prepareComplete_;
	bool analyseComplete_;
	bool buildComplete_;
	bool cleanComplete_;
	bool distCleanComplete_;

	bool analyseResult_;
	bool buildResult_;
};

} // namespace mach

#endif // MACH_BUILDPLAN_HPP
