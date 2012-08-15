#ifndef FTL_GCCCOMPILER_HPP
#define FTL_GCCCOMPILER_HPP

#include "MachCompiler.hpp"

namespace ftl
{

class GccCompiler: public MachCompiler
{
public:
	inline static Ref<GccCompiler, Owner> newInstance(int options = 0, String execPath = "") {
		return new GccCompiler(options, execPath);
	}

	virtual Ref<MachObject, Owner> analyse(String source);
	virtual bool compile(Ref<MachObject, Owner> object);
	virtual bool link(Ref<MachObjectList> objectList);

protected:
	GccCompiler(int options, String execPath);

	static String lookup(String execPath);
};

} // namespace ftl

#endif // FTL_GCCCOMPILER_HPP
