#ifndef MACH_MODULE_HPP
#define MACH_MODULE_HPP

#include <ftl/string.hpp>
#include <ftl/Time.hpp>

namespace mach
{

using namespace ftl;

class Module: public Instance
{
public:
	inline static hook<Module> create(string analyseCommand, string modulePath, StringList *dependencyPaths, bool dirty) {
		return new Module(analyseCommand, modulePath, dependencyPaths, dirty);
	}

	inline string analyseCommand() const { return analyseCommand_; }
	inline string modulePath() const { return modulePath_; }
	inline string sourcePath() const { return dependencyPaths_->at(0); }
	inline string toolName() const { return sourcePath()->baseName(); }
	inline StringList *dependencyPaths() const { return dependencyPaths_; }
	inline bool dirty() const { return dirty_; }

private:
	Module(string analyseCommand, string modulePath, StringList *dependencyPaths, bool dirty)
		: analyseCommand_(analyseCommand),
		  modulePath_(modulePath),
		  dependencyPaths_(dependencyPaths),
		  dirty_(dirty)
	{}

	string analyseCommand_;
	string modulePath_;
	hook<StringList> dependencyPaths_;
	bool dirty_;
};

typedef List< hook<Module> > ModuleList;

} // namespace mach

#endif // MACH_MODULE_HPP
