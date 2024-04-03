/*
 * Copyright (C) 2007-2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/TokiHtmlScreen>

namespace cc {

struct TokiHtmlScreen::State final: public TokenScreen::State
{
    State(const String &text, const TokiTheme &theme, const Format &sink):
        text_{text},
        sink_{sink}
    {
        TokiStyle style = theme.defaultPalette().textStyle();
        sink_ << "<pre style=\"color:" << style.ink() << ";background-color:" << style.paper() << "\">\n";
    }

    ~State()
    {
        sink_ << "</pre>\n";
    }

    bool project(const Token &token, long i0, long i1) override
    {
        String s = text_.copy(i0, i1);
        s.replace("<", "&lt;");
        s.replace(">", "&gt;");
        s.replace("\t", "    ");
        long spacesCount = 0;
        for (char ch: s) {
            spacesCount += (ch <= ' ');
        }
        if (spacesCount < s.count()) {
            Object metaData = token.rule().metaData();
            if (metaData) {
                TokiStyle style = metaData.as<TokiStyle>();
                sink_ << "<span style=\"";
                if (style.ink()) sink_ << "color:" << style.ink();
                if (style.paper()) sink_ << ";background-color:" << style.paper();
                if (style.bold()) sink_ << ";font-weight:bold";
                if (style.italic()) sink_ << ";font-style:italic";
                sink_ << "\">" << s << "</span>";
            }
            else {
                sink_ << s;
            }
        }
        else {
            sink_ << s;
        }
        return true;
    }

    String text_;
    Format sink_;
};

TokiHtmlScreen::TokiHtmlScreen(const String &text, const TokiTheme &theme, const Format &sink):
    TokenScreen{new State{text, theme, sink}}
{}

} // namespace cc
