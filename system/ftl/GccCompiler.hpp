#ifndef FTL_GCCCOMPILER_HPP
#define FTL_GCCCOMPILER_HPP

#include "MachCompiler.hpp"
#include "Format.hpp"

namespace ftl
{

class GccCompiler: public MachCompiler
{
public:
	inline static Ref<GccCompiler, Owner> newInstance(int options = 0, String execPath = "g++") {
		return new GccCompiler(options, execPath);
	}

	virtual Ref<MachObject, Owner> analyse(String source);
	virtual bool compile(Ref<MachObject, Owner> object);
	virtual bool link(Ref<MachObjectList> objectList);

protected:
	GccCompiler(int options, String execPath);
	void appendCompileOptions(Format args);

	static String lookup(String execPath);
};

} // namespace ftl

#endif // FTL_GCCCOMPILER_HPP
