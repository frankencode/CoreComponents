#ifndef FTL_MACHOBJECT_HPP
#define FTL_MACHOBJECT_HPP

#include "String.hpp"
#include "Time.hpp"

namespace ftl
{

class MachObject: public Instance
{
public:
	inline static Ref<MachObject, Owner> newInstance(String command, String objectPath, Ref<StringList> dependencyPaths, bool dirty) {
		return new MachObject(command, objectPath, dependencyPaths, dirty);
	}

	inline String command() const { return command_; }
	inline String objectPath() const { return objectPath_; }
	inline String sourcePath() const { return dependencyPaths_->at(0); }
	inline Ref<StringList> dependencyPaths() const { return dependencyPaths_; }
	inline bool dirty() const { return dirty_; }

private:
	MachObject(String command, String objectPath, Ref<StringList> dependencyPaths, bool dirty)
		: command_(command),
		  objectPath_(objectPath),
		  dependencyPaths_(dependencyPaths),
		  dirty_(dirty)
	{}

	String command_;
	String objectPath_;
	Ref<StringList, Owner> dependencyPaths_;
	bool dirty_;
};

typedef List< Ref<MachObject, Owner> > MachObjectList;

} // namespace ftl

#endif // FTL_MACHOBJECT_HPP
