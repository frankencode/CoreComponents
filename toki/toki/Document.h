/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXTOKI_DOCUMENT_H
#define FLUXTOKI_DOCUMENT_H

#include <flux/String.h>
#include "Style.h"

namespace fluxtoki
{

using namespace flux;

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

} // namespace fluxtoki

#endif // FLUXTOKI_DOCUMENT_H
