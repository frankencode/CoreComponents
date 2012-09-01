#ifndef FTL_MACHCOMPILER_HPP
#define FTL_MACHCOMPILER_HPP

#include "MachObject.hpp"

namespace ftl
{

FTL_STD_EXCEPTION(MachException);

class WireObject;

class MachCompiler: public Instance
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
	virtual Ref<MachObject, Owner> analyse(String source, int options = 0) = 0;
	virtual bool compile(Ref<MachObject, Owner> object, int options = 0) = 0;
	virtual bool link(Ref<MachObjectList> objects, Ref<StringList> libraries, String name, String version, int options = 0) = 0;

	bool build(Ref<WireObject> recipe);
	void clean(Ref<WireObject> recipe);

protected:
	MachCompiler();
	void init(String execPath, String machine);

	int optionsFromRecipe(Ref<WireObject> recipe) const;
	Ref<MachObjectList, Owner> analyse(Ref<WireObject> recipe);

private:
	String execPath_;
	String machine_;
};

} // namespace ftl

#endif // FTL_MACHCOMPILER_HPP
