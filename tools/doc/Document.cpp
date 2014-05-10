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

Ref<Document> Document::load(String path)
{
	String text = File::open(path)->map();
	Ref<FragmentList> fragments = markupSyntax()->parse(text, path);
	for (int i = 0; i < fragments->size(); ++i) ferr() << yason::stringify(fragments->at(i));
	return new Document(fragments);
}

Ref<Document> Document::create(FragmentList *fragments)
{
	return new Document(fragments);
}

Document::Document(FragmentList *fragments)
	: fragments_(fragments),
	  title_(Title::create()),
	  authors_(AuthorList::create()),
	  parts_(DocumentList::create())
{
	for (int i = 0; i < fragments->size(); ++i) {
		Fragment *fragment = fragments->at(i);
		String className = fragment->className();
		if (className == "Title") {
			title_ = fragment;
		}
		else if (className == "Author") {
			authors_->append(fragment);
		}
		else if (className == "Part") {
			Part *part = cast<Part>(fragment);
			Ref<Document> document = load(part->path());
			parts_->append(document);
		}
	}
}

} // namespace fluxdoc
