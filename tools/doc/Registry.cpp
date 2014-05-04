#include "Registry.h"

namespace fluxdoc
{

Registry::Registry()
	: outputFormatByName_(OutputFormatByName::create())
{}

void Registry::registerOutputFormat(OutputFormat *format)
{
	outputFormatByName_->insert(format->name(), format);
}

OutputFormat* Registry::outputFormatByName(String name) const
{
	Ref<OutputFormat> format;
	outputFormatByName_->lookup(name, &format);
	return format;
}

OutputFormat* Registry::outputFormatByIndex(int index) const
{
	return outputFormatByName_->valueAt(index);
}

int Registry::outputFormatCount() const
{
	return outputFormatByName_->size();
}

} // namespace fluxdoc
