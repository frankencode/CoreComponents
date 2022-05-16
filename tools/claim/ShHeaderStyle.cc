/*
 * Copyright (C) 2007-2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/claim/ShHeaderStyle>
#include <cc/SyntaxDefinition>
#include <cc/File>
#include <cc/Format>
#include <cc/Registration>

namespace cc::claim {

class ShHeaderSyntax final: public SyntaxDefinition
{
public:
    ShHeaderSyntax():
        SyntaxDefinition{new State}
    {}

private:
    struct State final: public SyntaxDefinition::State
    {
        SyntaxRule magic_ {
            Sequence{
                Repeat{OneOf{" \t"}},
                "#!",
                Find{'\n'}
            }
        };

        SyntaxRule header_ {
            Sequence{
                Repeat{1,
                    Sequence{
                        Repeat{OneOf{" \t"}},
                        '#',
                        Find{'\n'}
                    }
                },
                Repeat{OneOf{" \t\n\r"}}
            }
        };

        SyntaxRule script_ {
            Sequence{
                Repeat{0, 1, Inline{&magic_}},
                &header_
            }
        };

        State():
            SyntaxDefinition::State{&script_}
        {}
    };
};

class ShMagicSyntax final: public SyntaxDefinition
{
public:
    ShMagicSyntax():
        SyntaxDefinition{new State}
    {}

private:
    struct State final: public SyntaxDefinition::State
    {
        SyntaxRule magic_ {
            Sequence{
                Repeat{OneOf{" \t"}},
                "#!",
                Find{'\n'}
            }
        };

        State():
            SyntaxDefinition::State{&magic_}
        {}
    };
};

struct ShHeaderStyle::State final: public HeaderStyle::State
{
    State():
        HeaderStyle::State{"sh"}
    {}

    Header scan(const String &path) const override
    {
        String text = File{path}.map();
        Token rootToken = headerSyntax_.match(text);
        if (!rootToken) return Header{};
        Token token = rootToken.children().first();
        String message = trimHeader(text.copy(token), " \t\r#");
        return Header{path, token, text, message};
    }

    String str(const Notice &notice) const override
    {
        Format f;
        f << "##\n";
        for (const Copyright &c: notice.copyrights()) {
            f << " # Copyright (C) ";
            if (c.yearStart() == c.yearEnd()) f << c.yearStart();
            else f << c.yearStart() << "-" << c.yearEnd();
            f << " " << c.holder() << ".\n";
        }
        f <<
            " #\n"
            " # " << notice.statement().replaced("\n", "\n # ") << "\n"
            " #\n"
            " ##\n"
            "\n";
        return f;
    }

    long magicCount(const String &text) const override
    {
        Token token = magicSyntax_.match(text);
        return (token) ? token.i1() : 0;
    }

    ShHeaderSyntax headerSyntax_;
    ShMagicSyntax magicSyntax_;
};

ShHeaderStyle::ShHeaderStyle():
    HeaderStyle{new State}
{}

CC_REGISTRATION(ShHeaderStyle);

} // namespace cc::claim
