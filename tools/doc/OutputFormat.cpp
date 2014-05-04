#include "OutputFormat.h"

namespace fluxdoc
{

OutputFormat::OutputFormat(String name)
	: name_(name)
{}

String OutputFormat::name() const { return name_; }

} // namespace fluxdoc
