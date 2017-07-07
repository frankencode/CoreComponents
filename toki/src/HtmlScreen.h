/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Format>
#include <cc/syntax/Token>

namespace cc {
namespace toki {

using namespace cc::syntax;

class Theme;

class HtmlScreen: public Object, public TokenScreen
{
public:
    static Ref<HtmlScreen> create(String text, Format sink = Format()) { return new HtmlScreen(text, sink); }

    virtual bool project(Token *token, int i0, int i1);

    static void writeCss(Theme *theme, Stream *sink = 0);

private:
    HtmlScreen(String text, Format sink);
    ~HtmlScreen();

    void writeLineNumbers();

    String text_;
    Format sink_;
};

}} // namespace cc::toki

