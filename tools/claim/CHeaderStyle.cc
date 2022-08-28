/*
 * Copyright (C) 2007-2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/claim/CHeaderStyle>
#include <cc/SyntaxDefinition>
#include <cc/File>
#include <cc/Format>
#include <cc/Registration>

namespace cc::claim {

class CHeaderSyntax final: public SyntaxDefinition
{
public:
    CHeaderSyntax():
        SyntaxDefinition{new State}
    {}

private:
    struct State final: public SyntaxDefinition::State
    {
        SyntaxRule comment_ {
            Sequence{
                "/*",
                Find{"*/"}
            }
        };

        SyntaxRule header_ {
            Sequence{
                Repeat{OneOf{" \t\n\r"}},
                &comment_,
                Repeat{OneOf{" \t\n\r"}}
            }
        };

        State():
            SyntaxDefinition::State{&header_}
        {}
    };
};

struct CHeaderStyle::State final: public HeaderStyle::State
{
    State():
        HeaderStyle::State("cxx")
    {}

    Header scan(const String &path) const override
    {
        String text = File{path}.map();
        Token rootToken = headerSyntax_.match(text);
        if (!rootToken) return Header{};
        Token token = rootToken.children().first();
        String message = trimHeader(text.copy(token.i0() + 2, token.i1() - 2), " \t\r*");
        return Header{path, rootToken, text, message};
    }

    String str(const Notice &notice) const override
    {
        Format f;
        f << "/*\n";
        for (const Copyright &c: notice.copyrights()) {
            f << " * Copyright (C) ";
            if (c.yearStart() == c.yearEnd()) f << c.yearStart();
            else f << c.yearStart() << "-" << c.yearEnd();
            f << " " << c.holder() << ".";
            if (!notice.copyrights().isLast(c)) f << "\n";
        }
        f << "\n"
            " *\n"
            " * " << notice.statement().replaced("\n", "\n * ") << "\n"
            " *\n"
            " */\n"
            "\n";
        return f;
    }

    CHeaderSyntax headerSyntax_;
};

CHeaderStyle::CHeaderStyle():
    HeaderStyle{new State}
{}

CC_REGISTRATION(CHeaderStyle);

} // namespace cc::claim
