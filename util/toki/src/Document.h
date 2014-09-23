/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXTOKI_DOCUMENT_H
#define FLUXTOKI_DOCUMENT_H

#include <flux/String.h>
#include "Style.h"

namespace flux {
namespace toki {

class Span: public Range
{
public:
	inline static Ref<Span> create(int i0, int i1, Style *style = 0) {
		return new Span(i0, i1, style);
	}

	inline Style *style() const { return style_; }

protected:
	Span(int i0, int i1, Style *style = 0)
		: Range(i0, i1),
		  style_(style)
	{}

	Ref<Style> style_;
};

class Document: public Object
{
public:
	static Ref<Document> load(String path);

	inline static Ref<Document> create(String text, String path = "") {
		return new Document(text, path);
	}

	inline String text() const { return text_; }
	inline String path() const { return path_; }
	void save();

protected:
	Document(String text, String path);

	String path_;
	String text_;
	typedef List< Ref<Span> > Spans;
	Ref<Spans> spans_;
};

}} // namespace flux::toki

#endif // FLUXTOKI_DOCUMENT_H
