#ifndef FTL_BUILDPLAN_HPP
#define FTL_BUILDPLAN_HPP

#include <ftl/PrefixTree.hpp>
#include "Module.hpp"
#include "ToolChain.hpp"

namespace ftl
{

class FileStatus;
class Config;
class BuildPlan;

typedef List< Ref<BuildPlan, Owner> > BuildPlanList;

FTL_EXCEPTION(BuildPlanException, Exception);

class BuildPlan: public Instance
{
public:
	enum Option {
		Static        = 1,
		Library       = 2,
		ToolSet       = 4,
		Package       = 8,
		Debug         = 16,
		Release       = 32,
		OptimizeSpeed = 64,
		OptimizeSize  = 128,
		DryRun        = 256,
		Blindfold     = 512,
		Bootstrap     = 1024,
		Verbose       = 2048,
		Unspecified   = -1,
		GlobalOptions = Debug|Release|OptimizeSpeed|OptimizeSize|DryRun|Blindfold|Bootstrap|Verbose
	};

	static Ref<BuildPlan, Owner> create(int argc, char **argv);

	inline Ref<ToolChain> toolChain() const { return toolChain_; }
	inline String projectPath() const { return projectPath_; }
	inline String modulePath() const { return modulePath_; }
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

	String sourcePath(String source) const;
	String modulePath(String object) const;

	String beautifyCommand(String command);
	String runAnalyse(String command);
	bool runBuild(String command);
	bool mkdir(String path);
	bool rmdir(String path);
	bool symlink(String path, String newPath);
	bool unlink(String path);
	Ref<FileStatus, Owner> fileStatus(String path);

private:
	Ref<BuildPlan, Owner> create(Ref<ToolChain> toolChain, String projectPath, int globalOptions);

	typedef PrefixTree< char, Ref<BuildPlan> > BuildMap;

	BuildPlan(int argc, char **argv);
	BuildPlan(Ref<ToolChain> toolChain, String projectPath, int globalOptions, Ref<BuildMap> buildMap);

	void readRecipe(int globalOptions = Unspecified);

	void prepare();
	void analyse();
	bool build();
	void clean();

	Ref<ToolChain, Owner> toolChain_;
	String projectPath_;
	String modulePath_;
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

	Ref<BuildMap, Owner> buildMap_;
	String sourcePrefix_;

	bool prepareComplete_;
	bool analyseComplete_;
	bool buildComplete_;
	bool cleanComplete_;
	bool distCleanComplete_;

	bool buildResult_;
};

} // namespace ftl

#endif // FTL_BUILDPLAN_HPP
