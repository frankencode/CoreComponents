/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <flux/stdio.h> // DEBUG
#include <flux/yason.h> // DEBUG
#include "MarkupSyntax.h"
#include "Document.h"

namespace fluxdoc
{

Ref<Document> Document::load(String path, Document *parent)
{
	return Document::parse(File::open(path)->map(), path, parent);
}

Ref<Document> Document::parse(String text, String path, Document *parent)
{
	Ref<ElementList> elements = markupSyntax()->parse(text, path);
	for (int i = 0; i < elements->count(); ++i) ferr() << yason::stringify(elements->at(i)) << nl; // DEBUG
	return new Document(elements, path, parent);
}

int Document::depth() const
{
	int n = 0;
	for (Document *d = parent_; d; d = d->parent_) ++n;
	return n;
}

Document::Document(ElementList *elements, String path, Document *parent)
	: path_(path),
	  elements_(elements),
	  title_(TitleElement::create()),
	  authors_(AuthorList::create()),
	  parent_(parent),
	  parts_(DocumentList::create())
{
	for (int i = 0; i < elements->count(); ++i) {
		Element *element = elements->at(i);
		String className = element->className();
		if (className == "Title") {
			title_ = element;
		}
		else if (className == "Author") {
			authors_->append(element);
		}
		else if (className == "Part") {
			PartElement *part = cast<PartElement>(element);
			Ref<Document> document = load(part->path(), this);
			parts_->append(document);
		}
	}
}

} // namespace fluxdoc
