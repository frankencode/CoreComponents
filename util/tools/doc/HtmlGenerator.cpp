/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/Format>
#include <flux/File>
#include <flux/toki/HtmlScreen>
#include "HtmlDesign.h"
#include "HtmlGenerator.h"

namespace flux {
namespace doc {

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

    virtual void write(Format &sink, Element *element)
    {
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

    virtual void write(Format &sink, Element *element)
    {
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

    virtual void write(Format &sink, Element *element)
    {
        CodeElement *code = cast<CodeElement>(element);
        sink << "<figure class=\"code\">" << nl;
        if (code->title() != "")
            sink << "<figcaption>" << code->title() << "</figcaption>" << nl;
        code->state()->rootToken()->project(toki::HtmlScreen::create(code->text(), sink));
        sink << "</figure>" << nl;
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
    Ref<HtmlDesign> htmlDesign = design;
    ElementList *elements = document->elements();

    String baseName = document->path()->fileName()->baseName();
    String path = baseName + ".html";
    Ref<File> file = File::open(path, File::WriteOnly|File::Truncate|File::Create);

    {
        Ref<File> file = File::open(baseName + ".css", File::Create|File::Truncate|File::WriteOnly);
        Format sink(file);
        sink <<
            "body {\n"
            "  margin: 1em auto;\n"
            "  width: 600px;\n"
            "  line-height: 1.5;\n"
            "  font-size: 16px;\n"
            "  font-family: sans-serif;\n"
            "}\n";
    }

    bool needTokiTheme = false;
    for (int i = 0; i < elements->count(); ++i) {
        if (elements->at(i)->className() == "Code") { needTokiTheme = true; break; }
    }
    if (needTokiTheme) {
        Ref<File> file = File::open(htmlDesign->tokiTheme()->name() + ".css", File::Create|File::Truncate|File::WriteOnly);
        toki::HtmlScreen::writeCss(htmlDesign->tokiTheme(), file);
    }

    Format sink(file);
    sink
        << "<!DOCTYPE HTML>" << nl
        << "<html>" << nl
        // FIXME << "<html lang=\"" << document->locale() << "\">"
        << "<head>" << nl
        << "<meta charset=\"UTF-8\">" << nl;

    sink << "<link rel=\"stylesheet\" href=\"" << baseName + ".css" << "\" type=\"text/css\">" << nl;

    if (needTokiTheme)
        sink << "<link rel=\"stylesheet\" href=\"" << htmlDesign->tokiTheme()->name() << ".css\" type=\"text/css\">" << nl;

    sink
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
