#ifndef FTL_MACHOBJECT_HPP
#define FTL_MACHOBJECT_HPP

#include "String.hpp"
#include "Format.hpp"

namespace ftl
{

class File;
class WireObject;

class MachObject: public Instance
{
public:
	MachObject(String objectPath, String sourcePath, Ref<StringList> dependencyPaths);

	inline String objectPath() const { return objectPath_; }
	inline String sourcePath() const { return sourcePath_; }
	inline Ref<StringList> dependencyPaths() const { return dependencyPaths_; }

	static Ref<MachObject, Owner> parse(Ref<WireObject> wireObject);
	void stringifyTo(Format text, String indent);

	Ref<File> sourceFile();

private:
	String objectPath_;
	String sourcePath_;
	Ref<StringList, Owner> dependencyPaths_;
	Ref<File, Owner> sourceFile_;
};

typedef List<MachObject> MachObjectList;

} // namespace ftl

#endif // FTL_MACHOBJECT_HPP
