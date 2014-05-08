#include "Registry.h"
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

int HtmlFormatRegistration::count_ = 0;

HtmlFormatRegistration::HtmlFormatRegistration()
{
	if (count_ == 0) {
		++count_;
		registry()->registerOutputFormat(HtmlFormat::create());
	}
}

} // namespace fluxdoc
