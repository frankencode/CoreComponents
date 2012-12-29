#ifndef FTL_MODULE_HPP
#define FTL_MODULE_HPP

#include <ftl/String.hpp>
#include <ftl/Time.hpp>

namespace mach
{

using namespace ftl;

class Module: public Instance
{
public:
	inline static Ref<Module, Owner> create(String analyseCommand, String modulePath, Ref<StringList> dependencyPaths, bool dirty) {
		return new Module(analyseCommand, modulePath, dependencyPaths, dirty);
	}

	inline String analyseCommand() const { return analyseCommand_; }
	inline String modulePath() const { return modulePath_; }
	inline String sourcePath() const { return dependencyPaths_->at(0); }
	inline String toolName() const { return sourcePath()->baseName(); }
	inline Ref<StringList> dependencyPaths() const { return dependencyPaths_; }
	inline bool dirty() const { return dirty_; }

private:
	Module(String analyseCommand, String modulePath, Ref<StringList> dependencyPaths, bool dirty)
		: analyseCommand_(analyseCommand),
		  modulePath_(modulePath),
		  dependencyPaths_(dependencyPaths),
		  dirty_(dirty)
	{}

	String analyseCommand_;
	String modulePath_;
	Ref<StringList, Owner> dependencyPaths_;
	bool dirty_;
};

typedef List< Ref<Module, Owner> > ModuleList;

} // namespace mach

#endif // FTL_MODULE_HPP
