/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <flux/Format.h>
#include <flux/File.h>
#include "Design.h"
#include "HtmlGenerator.h"

namespace flux {
namespace doc {

class HtmlDesign: public Design
{
public:
	static Ref<HtmlDesign> create() { return new HtmlDesign; }

private:
	HtmlDesign()
		: Design("Html")
	{}

	virtual Ref<MetaObject> produce() {
		return create();
	}
};

class HtmlWriter: public Object {
public:
	virtual void write(Format &sink, Element *element) = 0;
};

namespace html
{

class ParagraphWriter: public HtmlWriter {
public:
	static Ref<ParagraphWriter> create() { return new ParagraphWriter; }
	static const char *className() { return "Paragraph"; }
	virtual void write(Format &sink, Element *element) {
		ParagraphElement *paragraph = cast<ParagraphElement>(element);
		sink << "<p>" << paragraph->text() << "</p>" << nl;
	}
private:
	ParagraphWriter() {}
};

class ListWriter: public HtmlWriter {
public:
	static Ref<ListWriter> create() { return new ListWriter; }
	static const char *className() { return "List"; }
	virtual void write(Format &sink, Element *element) {
		ListElement *list = cast<ListElement>(element);
		if (!list->hasChildren()) return;
		if (list->children()->count() == 0) return;
		sink << "<ul>" << nl;
		for (int i = 0; i < list->children()->count(); ++i) {
			ItemElement *item = cast<ItemElement>(list->children()->at(i));
			sink << "<li>" << item->text();
			if (item->hasChildren()) {
				sink << nl;
				write(sink, cast<Element>(item->children()->at(0)));
			}
			sink << "</li>" << nl;
		}
		sink << "</ul>" << nl;
	}
};

class CodeWriter: public HtmlWriter {
public:
	static Ref<CodeWriter> create() { return new CodeWriter; }
	static const char *className() { return "Code"; }
	virtual void write(Format &sink, Element *element) {
		CodeElement *code = cast<CodeElement>(element);
		String text = code->text();
		if (code->path() != "") text = File::open(code->path())->map();
		sink << "<pre>" << nl;
		sink << text;
		sink << "</pre>" << nl;
	}
};

} // namespace html

HtmlGenerator::HtmlGenerator()
	: Generator("Html", HtmlDesign::create()),
	  writerMap_(WriterMap::create())
{
	using namespace html;

	define<ParagraphWriter>();
	define<ListWriter>();
	define<CodeWriter>();
}

void HtmlGenerator::run(Design *design, Document *document)
{
	String path = document->path()->fileName()->baseName() + ".html";
	Ref<File> file = File::open(path, File::WriteOnly|File::Truncate|File::Create);

	Format sink(file);
	sink
		<< "<!DOCTYPE HTML>" << nl
		<< "<html>" << nl
		// FIXME << "<html lang=\"" << document->locale() << "\">"
		<< "<head>" << nl
		<< "<meta charset=\"UTF-8\">" << nl
		<< "<title>" << document->title()->text() << "</title>" << nl
		<< "</head>" << nl
		<< "<body>" << nl;

	sink << "<header>" << nl;
	if (document->title()->text() != "") {
		int h = document->depth() + 1;
		if (h > 6) h = 6;
		sink << "<h" << h << ">" << document->title()->text() << "</h" << h << ">" << nl;
	}
	sink << "</header>" << nl;

	ElementList *elements = document->elements();
	for (int i = 0; i < elements->count(); ++i) {
		Element *element = elements->at(i);
		Ref<HtmlWriter> writer;
		if (!writerMap_->lookup(element->className(), &writer)) continue;
		writer->write(sink, element);
	}

	sink
		<< "</body>" << nl
		<< "</html>" << nl;
}

}} // namespace flux::doc
