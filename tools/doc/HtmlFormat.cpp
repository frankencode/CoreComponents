#include "HtmlFormat.h"

namespace fluxdoc
{

Ref<HtmlFormat> HtmlFormat::create()
{
	return new HtmlFormat;
}

HtmlFormat::HtmlFormat()
	: OutputFormat("html")
{}

void HtmlFormat::write(Stream *stream, Document *document) const
{
}

} // namespace fluxdoc
