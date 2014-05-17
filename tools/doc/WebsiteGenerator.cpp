/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <flux/Format.h>
#include "Design.h"
#include "WebsiteGenerator.h"

namespace fluxdoc
{

class WebsiteDesign: public Design
{
public:
	static Ref<WebsiteDesign> create() { return new WebsiteDesign; }

private:
	WebsiteDesign()
		: Design("Website")
	{}

	virtual Ref<YasonObject> produce() {
		return create();
	}
};

class WebsiteWriter: public Object {
public:
	virtual void write(Format &sink, const Fragment *fragment) = 0;
};

class ParagraphWriter: public WebsiteWriter {
public:
	static Ref<ParagraphWriter> create() { return new ParagraphWriter; }
	static const char *className() { return "Paragraph"; }
	virtual void write(Format &sink, const Fragment *fragment) {
		const Paragraph *paragraph = cast<const Paragraph>(fragment);
		sink << "<p>" << paragraph->text() << "</p>" << nl;
	}
private:
	ParagraphWriter() {}
};

/*class ItemWriter: public ItemWriter {
public:
	static Ref<ItemWriter> create() { return new ItemWriter; }
	static const char *className() const { return "Item"; }
	virtual void write(Format &sink, const Fragment *fragment) {
		Item *item = cast<const Item>(fragment);
		if (item->depth() > currentDepth_)
			sink << "<ul>" << nl;
		if (
	}
private:
	ItemWriter(): currentDepth_(-1) {}
	int currentDepth_;
};*/

WebsiteGenerator::WebsiteGenerator()
	: Generator("Website", WebsiteDesign::create()),
	  writerMap_(WriterMap::create())
{
	define<ParagraphWriter>();
}

void WebsiteGenerator::run(Design *design, Document *document)
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

	FragmentList *fragments = document->fragments();
	for (int i = 0; i < fragments->size(); ++i) {
		Fragment *fragment = fragments->at(i);
		Ref<WebsiteWriter> writer;
		if (!writerMap_->lookup(fragment->className(), &writer)) continue;
		writer->write(sink, fragment);
	}

	sink
		<< "</body>" << nl
		<< "</html>" << nl;
}

} // namespace fluxdoc
