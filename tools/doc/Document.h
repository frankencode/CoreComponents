#ifndef FLUXDOC_DOCUMENT_H
#define FLUXDOC_DOCUMENT_H

#include "Fragment.h"

namespace fluxdoc
{

using namespace flux;

class Document: public Object
{
public:
	static Ref<Document> create(FragmentList *fragments);

private:
	Document(FragmentList *fragments);

	Ref<FragmentList> fragments_;
};

} // namespace fluxdoc

#endif // FLUXDOC_DOCUMENT_H
