#ifndef FLUXDOC_OUTPUTFORMAT_H
#define FLUXDOC_OUTPUTFORMAT_H

#include <flux/Object.h>
#include <flux/String.h>
#include <flux/Stream.h>
#include "Document.h"

namespace fluxdoc
{

using namespace flux;

class OutputFormat: public Object
{
public:
	String name() const;
	virtual void write(Stream *stream, Document *document) const = 0;

protected:
	OutputFormat(String name);

private:
	String name_;
};

} // namespace fluxdoc

#endif // FLUXDOC_OUTPUTFORMAT_H
