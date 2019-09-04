/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Singleton>
#include <cc/syntax/SyntaxDefinition>
#include <cc/stdio>
#include "NoticeParser.h"

namespace ccclaim {

using namespace cc::syntax;

class NoticeSyntax: public SyntaxDefinition
{
public:
    NoticeSyntax();

    Ref<Notice> readNotice(Header *header) const;
    Ref<Copyright> readCopyright(Token *token, String message) const;

private:
    int word_;
    int yearStart_;
    int yearEnd_;
    int holder_;
    int description_;
    int copyright_;
    int statement_;
    int notice_;
};

NoticeSyntax::NoticeSyntax()
{
    DEFINE("Year",
        GLUE(
            RANGE('1', '3'),
            REPEAT(3, 3,
                RANGE('0', '9')
            )
        )
    );

    word_ =
        DEFINE("Word",
            REPEAT(1,
                CHOICE(
                    RANGE('a', 'z'),
                    RANGE('A', 'Z'),
                    GREATER(127)
                )
            )
        );

    yearStart_ = DEFINE("YearStart", INLINE("Year"));
    yearEnd_ = DEFINE("YearEnd", INLINE("Year"));

    holder_ =
        DEFINE("Holder",
            REPEAT(2, 3,
                GLUE(
                    NOT(STRING("All")),
                    REF("Word"),
                    REPEAT(0, 1, CHAR('.')),
                    REPEAT(0, 2, CHAR(' '))
                )
            )
        );

    description_ =
        DEFINE("Description",
            REPEAT(
                GLUE(
                    NOT(INLINE("Copyright")),
                    ANY()
                )
            )
        );

    copyright_ =
        DEFINE("Copyright",
            GLUE(
                REPEAT(RANGE(" \t")),
                STRING("Copyright"),
                REPEAT(RANGE(" \t")),
                REPEAT(0, 1,
                    CHOICE(
                        STRING("(C)"),
                        STRING("(c)"),
                        STRING("©")
                    )
                ),
                REPEAT(RANGE(" \t,")),
                REF("YearStart"),
                REPEAT(RANGE(" -")),
                REPEAT(0, 1, REF("YearEnd")),
                REPEAT(RANGE(" \t,")),
                REF("Holder"),
                REPEAT(RANGE(" \t.")),
                REPEAT(OTHER('\n')),
                CHAR('\n')
            )
        );

    statement_ =
        DEFINE("Statement",
            REPEAT(1,
                GLUE(
                    NOT(
                        GLUE(
                            REPEAT(RANGE(" \t\n\r")),
                            EOI()
                        )
                    ),
                    ANY()
                )
            )
        );

    notice_ =
        DEFINE("Notice",
            GLUE(
                REPEAT(RANGE(" \t\n\r")),
                REF("Description"),
                REPEAT(1,
                    GLUE(
                        REF("Copyright"),
                        REPEAT(RANGE(" \t\n\r"))
                    )
                ),
                REPEAT(RANGE(" \t\n\r")),
                REF("Statement"),
                REPEAT(RANGE(" \t\n\r"))
            )
        );

    ENTRY("Notice");

    LINK();
}

Ref<Notice> NoticeSyntax::readNotice(Header *header) const
{
    String message = header->message();
    Ref<Token> rootToken = match(message)->rootToken();
    if (!rootToken) return Ref<Notice>();
    Token *token = rootToken->firstChild();
    CC_ASSERT(token);
    token = token->nextSibling();
    Ref<CopyrightList> copyrights = CopyrightList::create();
    while (token) {
        if (token->rule() != copyright_) break;
        copyrights->append(readCopyright(token, message));
        token = token->nextSibling();
    }
    CC_ASSERT(token->rule() == statement_);
    String statement = message->copyRange(token);
    return Notice::create(copyrights, statement, header);
}

Ref<Copyright> NoticeSyntax::readCopyright(Token *token, String message) const
{
    CC_ASSERT(token->rule() == copyright_);
    token = token->firstChild();
    CC_ASSERT(token->rule() == yearStart_);
    int yearStart = message->copyRange(token)->toNumber<int>();
    int yearEnd = yearStart;
    token = token->nextSibling();
    CC_ASSERT(token);
    if (token->rule() == yearEnd_) {
        yearEnd = message->copyRange(token)->toNumber<int>();
        token = token->nextSibling();
    }
    CC_ASSERT(token);
    CC_ASSERT(token->rule() == holder_);
    token = token->firstChild();
    CC_ASSERT(token);
    Ref<StringList> parts = StringList::create();
    while (token) {
        CC_ASSERT(token->rule() == word_);
        parts->append(message->copyRange(token));
        token = token->nextSibling();
    }
    String holder = parts->join(" ");
    return Copyright::create(holder, yearStart, yearEnd);
}

NoticeParser::NoticeParser()
    : noticeSyntax_(new NoticeSyntax)
{}

Ref<Notice> NoticeParser::readNotice(Header *header) const
{
    return noticeSyntax_->readNotice(header);
}

const NoticeParser *noticeParser()
{
    return Singleton<NoticeParser>::instance();
}

} // namespace ccclaim
