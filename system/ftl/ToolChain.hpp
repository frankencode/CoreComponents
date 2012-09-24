#ifndef FTL_TOOLCHAIN_HPP
#define FTL_TOOLCHAIN_HPP

#include "Module.hpp"

namespace ftl
{

FTL_STD_EXCEPTION(MachException);

class Config;

class ToolChain: public Instance
{
public:
	enum Option {
		Debug         = 1,
		Release       = 2,
		Static        = 4,
		Library       = 8,
		OptimizeSpeed = 16,
		OptimizeSize  = 32
	};

	inline String execPath() const { return execPath_; }
	inline String machine() const { return machine_; }

	virtual String analyseCommand(String source, int options = 0) const = 0;
	virtual Ref<Module, Owner> analyse(String source, int options = 0) = 0;
	virtual bool compile(Ref<Module, Owner> module, int options = 0) = 0;
	virtual String linkPath(String name, String version, int options) const = 0;
	virtual bool link(Ref<ModuleList> modules, Ref<StringList> libraries, String name, String version, int options = 0) = 0;

	bool build(Ref<Config> recipe);
	void clean(Ref<Config> recipe);

protected:
	ToolChain();
	void init(String execPath, String machine);

	int optionsFromRecipe(Ref<Config> recipe) const;
	Ref<ModuleList, Owner> analyse(Ref<Config> recipe);

private:
	String execPath_;
	String machine_;
};

} // namespace ftl

#endif // FTL_TOOLCHAIN_HPP
