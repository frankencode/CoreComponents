#ifndef MACH_MODULE_HPP
#define MACH_MODULE_HPP

#include <ftl/String.hpp>
#include <ftl/Time.hpp>

namespace mach
{

using namespace ftl;

class Module: public Instance
{
public:
	inline static O<Module> create(String analyseCommand, String modulePath, StringList *dependencyPaths, bool dirty) {
		return new Module(analyseCommand, modulePath, dependencyPaths, dirty);
	}

	inline String analyseCommand() const { return analyseCommand_; }
	inline String modulePath() const { return modulePath_; }
	inline String sourcePath() const { return dependencyPaths_->at(0); }
	inline String toolName() const { return sourcePath()->baseName(); }
	inline StringList *dependencyPaths() const { return dependencyPaths_; }
	inline bool dirty() const { return dirty_; }

private:
	Module(String analyseCommand, String modulePath, StringList *dependencyPaths, bool dirty)
		: analyseCommand_(analyseCommand),
		  modulePath_(modulePath),
		  dependencyPaths_(dependencyPaths),
		  dirty_(dirty)
	{}

	String analyseCommand_;
	String modulePath_;
	O<StringList> dependencyPaths_;
	bool dirty_;
};

typedef List< O<Module> > ModuleList;

} // namespace mach

#endif // MACH_MODULE_HPP
