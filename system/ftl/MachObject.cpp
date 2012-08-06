#include "File.hpp"
#include "WireObject.hpp"
#include "MachObject.hpp"

namespace ftl
{

MachObject::MachObject(String objectPath, String sourcePath, Ref<StringList> dependencyPaths)
	: objectPath_(objectPath),
	  sourcePath_(sourcePath),
	  dependencyPaths_(dependencyPaths)
{}

Ref<MachObject, Owner> MachObject::parse(Ref<WireObject> wireObject)
{
	String objectPath = wireObject->value("objectPath");
	String sourcePath = wireObject->value("sourcePath");
	Format dependencyPaths;
	{
		Ref<VariantList, Owner> h = wireObject->value("dependencyPaths");
		for (int i = 0; i < h->length(); ++i)
			dependencyPaths << h->at(i);
	}
	return new MachObject(objectPath, sourcePath, dependencyPaths);
}

void MachObject::stringifyTo(Format text, String indent)
{
	text
		<< indent << "MachObject {\n"
		<< indent << "    objectPath: \"" << objectPath_ << "\"\n"
		<< indent << "    sourcePath: \"" << sourcePath_ << "\"\n"
		<< indent << "    dependencyPaths: [\n";
	for (int i = 0; i < dependencyPaths_->length(); ++i)
		text << indent << "    \"" << dependencyPaths_->at(i) << "\"\n";
	text
		<< indent << "    ]\n"
		<< indent << "}\n";
}

Ref<File> MachObject::sourceFile()
{
	if (!sourceFile_) sourceFile_ = new File(sourcePath_);
	return sourceFile_;
}

} // namespace ftl
