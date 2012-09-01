#ifndef FTL_GCCCOMPILER_HPP
#define FTL_GCCCOMPILER_HPP

#include "MachCompiler.hpp"
#include "Format.hpp"

namespace ftl
{

class GccCompiler: public MachCompiler
{
public:
	inline static Ref<GccCompiler, Owner> newInstance(String execPath = "g++") {
		return new GccCompiler(execPath);
	}

	virtual String analyseCommand(String source, int options = 0) const;
	virtual Ref<MachObject, Owner> analyse(String source, int options = 0);
	virtual bool compile(Ref<MachObject, Owner> object, int options = 0);
	virtual bool link(Ref<MachObjectList> objects, Ref<StringList> libraries, String name, String version, int options = 0);

protected:
	GccCompiler(String execPath);
	void appendCompileOptions(Format args, int options, String objectPath = "") const;

	static String lookup(String execPath);
};

} // namespace ftl

#endif // FTL_GCCCOMPILER_HPP
