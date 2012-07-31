#ifndef FTL_MACHCOMPILER_HPP
#define FTL_MACHCOMPILER_HPP

#include "MachObject.hpp"

namespace ftl
{

class WireObject;

class MachCompiler: public Instance
{
public:
	enum Option {
		DebugInfo = 1,
		OptimizeSpeed = 2,
		OptimizeSize = 4,
		SharedLibrary = 8,
		StaticLibrary = 16
	};

	MachCompiler(int options = 0);

	inline String execPath() const { return execPath_; }
	inline String machine() const { return machine_; }
	inline int options() const { return options_; }

	virtual Ref<MachObject, Owner> analyse(String source)= 0;
	virtual bool compile(Ref<MachObject, Owner> object) = 0;
	virtual bool link(Ref<MachObjectList> objectList) = 0;

	bool build(Ref<WireObject> recipe);

protected:
	void init(String execPath, String machine);

private:
	String execPath_;
	String machine_;
	int options_;
};

} // namespace mach

#endif // FTL_MACHCOMPILER_HPP
