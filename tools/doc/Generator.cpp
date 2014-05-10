#include "Registry.h"
#include "Generator.h"

namespace fluxdoc
{

Generator::Generator(String name)
	: name_(name)
{
	registry()->registerGenerator(this);
}

} // namespace fluxdoc
