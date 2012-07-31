#ifndef FTL_MACHOBJECT_HPP
#define FTL_MACHOBJECT_HPP

#include "String.hpp"

namespace ftl
{

class MachObject: public Instance
{
public:
	MachObject(String object, String source, Ref<StringList> dependencies)
		: object_(object),
		  source_(source),
		  dependencies_(dependencies)
	{}

	inline String object() const { return object_; }
	inline String source() const { return source_; }
	inline Ref<StringList> dependencies() const { return dependencies_; }

private:
	String object_;
	String source_;
	Ref<StringList> dependencies_;
};

typedef List<MachObject> MachObjectList;

} // namespace ftl

#endif // FTL_MACHOBJECT_HPP
