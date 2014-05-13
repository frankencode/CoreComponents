/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXDOC_DOCUMENT_H
#define FLUXDOC_DOCUMENT_H

#include "fragments.h"

namespace fluxdoc
{

using namespace flux;

class Document;
typedef List< Ref<Document> > DocumentList;
typedef List< Ref<Author> > AuthorList;

class Document: public Object
{
public:
	static Ref<Document> load(String path);
	static Ref<Document> parse(String text, String resource = "");
	static Ref<Document> create(FragmentList *fragments);

	inline FragmentList *fragments() const;

	inline Title *title() const { return title_; }
	inline AuthorList *authors() const { return authors_; }

	inline DocumentList *parts() const { return parts_; }

private:
	Document(FragmentList *fragments);

	Ref<FragmentList> fragments_;
	Ref<Title> title_;
	Ref<AuthorList> authors_;

	Ref<DocumentList> parts_;
};

} // namespace fluxdoc

#endif // FLUXDOC_DOCUMENT_H
