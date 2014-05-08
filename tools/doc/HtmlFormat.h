#ifndef FLUXDOC_HTMLFORMAT_H
#define FLUXDOC_HTMLFORMAT_H

#include "OutputFormat.h"

namespace fluxdoc
{

using namespace flux;

class HtmlFormat: public OutputFormat
{
public:
	static Ref<HtmlFormat> create();

	virtual void write(Stream *stream, Document *document) const;

private:
	HtmlFormat();
};

class HtmlFormatRegistration {
public:
	HtmlFormatRegistration();
private:
	static int count_;
};

namespace {
	HtmlFormatRegistration registration_;
}

} // namespace fluxdoc

#endif // FLUXDOC_HTMLFORMAT_H
