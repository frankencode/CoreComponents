#include "FileStatus.hpp"
#include "MachObject.hpp"

namespace ftl
{

MachObject::MachObject(String objectPath, Ref<StringList> dependencyPaths, Time cacheTime)
	: objectPath_(objectPath),
	  dependencyPaths_(dependencyPaths),
	  dirty_(true)
{
	if (!cacheTime) return;
	Time objectTime = FileStatus::newInstance(objectPath_)->lastModified();
	for (int i = 0; i < dependencyPaths_->length(); ++i) {
		Time sourceTime = FileStatus::newInstance(dependencyPaths_->at(i))->lastModified();
		if ((sourceTime > cacheTime) || (sourceTime > objectTime)) return;
	}
	dirty_ = false;
}

} // namespace ftl
