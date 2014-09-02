/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXDOC_DOCUMENT_H
#define FLUXDOC_DOCUMENT_H

#include "elements.h"

namespace flux {
namespace doc {

class Document;
typedef List< Ref<Document> > DocumentList;
typedef List< Ref<AuthorElement> > AuthorList;

class Document: public Object
{
public:
	static Ref<Document> load(String path, Document *parent = 0);
	static Ref<Document> parse(String text, String path = "", Document *parent = 0);

	inline String path() const { return path_; }
	inline ElementList *elements() const { return elements_; }

	inline TitleElement *title() const { return title_; }
	inline AuthorList *authors() const { return authors_; }

	inline Document *parent() const { return parent_; }
	inline DocumentList *parts() const { return parts_; }

	int depth() const;

private:
	Document(ElementList *elements, String path, Document *parent = 0);

	String path_;
	Ref<ElementList> elements_;
	Ref<TitleElement> title_;
	Ref<AuthorList> authors_;

	Document *parent_;
	Ref<DocumentList> parts_;
};

}} // namespace flux::doc

#endif // FLUXDOC_DOCUMENT_H
