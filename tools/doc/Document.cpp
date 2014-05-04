#include "Document.h"

namespace fluxdoc
{

Ref<Document> Document::create(FragmentList *fragments)
{
	return new Document(fragments);
}

Document::Document(FragmentList *fragments)
	: fragments_(fragments)
{}

} // namespace fluxdoc
