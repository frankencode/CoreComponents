/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/String>
#include <cc/toki/Style>

namespace cc {
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

}} // namespace cc::toki

