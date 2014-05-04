#include "MarkupProtocol.h"
#include "fragments.h"

namespace fluxdoc
{

MarkupProtocol::MarkupProtocol()
{
	add<Title>();
	add<Author>();
	add<Heading>();
	add<Paragraph>();
	add<fluxdoc::Item>();

	add<Part>();
	add<Image>();
	add<Code>();
}

} // namespace fluxdoc
