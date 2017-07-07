/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/str>
#include <cc/toki/HtmlScreen>
#include <cc/toki/Theme>

namespace cc {
namespace toki {

HtmlScreen::HtmlScreen(String text, Format sink):
    text_(text),
    sink_(sink)
{
    writeLineNumbers();
    sink_ << "<div class=\"sourceCodeCell\">\n";
    sink_ << "<pre class=\"toki_" << hex(unsigned(Palette::defaultScope())) << "_" << hex(int(Palette::Text)) << "\">\n";
}

HtmlScreen::~HtmlScreen()
{
    sink_ << "</pre>\n";
    sink_ << "</div>\n";
}

void HtmlScreen::writeLineNumbers()
{
    sink_ << "<div class=\"lineNumbersCell\">\n";
    sink_ << "<pre class=\"toki_" << hex(unsigned(Palette::defaultScope())) << "_" << hex(int(Palette::LineNumber)) << "\">\n";
    int n = text_->count('\n') + 1;
    if (text_->count() > 0) n -= (text_->at(text_->count() - 1) == '\n');
    int w = dec(n)->count();
    for (int i = 1; i <= n; ++i) sink_ << " " << right(dec(i), w) << " " << "\n";
    sink_ << "</pre>\n";
    sink_ << "</div>\n";
}

bool HtmlScreen::project(Token *token, int i0, int i1)
{
    String s = text_->copy(i0, i1);
    if (s->contains('<')) s = s->replace("<", "&lt;");
    if (s->contains('>')) s = s->replace(">", "&gt;");
    if (s->contains('\t')) s = s->replace("\t", "    ");
    bool whitespace = s->countCharsIn(" \t\n\r") == s->count();
    if (!whitespace)
        sink_ << "<span class=\"toki_" << hex(unsigned(token->scope())) << "_" << hex(token->rule()) << "\">";
    sink_ << s;
    if (!whitespace)
        sink_ << "</span>";
    return true;
}

void HtmlScreen::writeCss(Theme *theme, Stream *sink)
{
    Format format(sink);
    int defaultScope = Palette::defaultScope();
    for (int i = 0; i < theme->paletteCount(); ++i) {
        const Palette *palette = theme->paletteAt(i);
        for (int j = 0; j < palette->styleCount(); ++j) {
            const Style *style = palette->styleAt(j);
            format << ".toki_" << hex(unsigned(palette->scope())) << "_" << hex(unsigned(style->rule())) << " { ";
            if (style->ink() != Color()) format << "color: " << style->ink() << "; ";
            if (style->paper() != Color()) format << "background-color: " << style->paper() << "; ";
            if (style->bold()) format << "font-weight: bold; ";
            if (style->italic()) format << "font-style: italic; ";
            if (palette->scope() == defaultScope) {
                if (style->rule() == Palette::Text) format << "margin: 0; padding: 0; margin-left: 0.3em; padding-top: 0.2em; padding-bottom: 0.2em; ";
                if (style->rule() == Palette::LineNumber) format << "margin: 0; padding: 0; padding-top: 0.2em; padding-bottom: 0.2em; -webkit-touch-callout: none; -webkit-user-select: none; -khtml-user-select: none; -moz-user-select: none; -ms-user-select: none; user-select: none; cursor: default; ";
            }
            format << "} /* " << palette->scopeName() << "::" << style->ruleName() << " */\n";
        }
    }
    format << ".lineNumbersCell { display: table-cell; margin: 0; padding: 0; -webkit-touch-callout: none; -webkit-user-select: none; -khtml-user-select: none; -moz-user-select: none; -ms-user-select: none; user-select: none; cursor: default; }\n";
    format << ".sourceCodeCell { display: table-cell; margin: 0; padding: 0; }\n";
}

}} // namespace cc::toki
