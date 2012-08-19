#ifndef FTL_MACHOBJECT_HPP
#define FTL_MACHOBJECT_HPP

#include "String.hpp"
#include "Time.hpp"

namespace ftl
{

class MachObject: public Instance
{
public:
	inline static Ref<MachObject, Owner> newInstance(String objectPath, Ref<StringList> dependencyPaths, Time cacheTime = Time()) {
		return new MachObject(objectPath, dependencyPaths, cacheTime);
	}

	inline String objectPath() const { return objectPath_; }
	inline String sourcePath() const { return dependencyPaths_->at(0); }
	inline Ref<StringList> dependencyPaths() const { return dependencyPaths_; }
	inline bool dirty() const { return dirty_; }

private:
	MachObject(String objectPath, Ref<StringList> dependencyPaths, Time cacheTime);

	String objectPath_;
	Ref<StringList, Owner> dependencyPaths_;
	bool dirty_;
};

typedef List< Ref<MachObject, Owner> > MachObjectList;

} // namespace ftl

#endif // FTL_MACHOBJECT_HPP
