/*
 * Copyright (C) 2007-2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/claim/NoticeSyntax>
#include <cassert>

namespace cc::claim {

struct NoticeSyntax::State final: public SyntaxDefinition::State
{
    SyntaxRule year_ {
        Sequence{
            Within{'1', '3'},
            Repeat{3, 3,
                Within{'0', '9'}
            }
        }
    };

    SyntaxRule word_ {
        Repeat{1,
            Choice{
                Within{'a', 'z'},
                Within{'A', 'Z'},
                Match{[](char ch){ return ch > 127; }}
            }
        }
    };

    SyntaxRule yearStart_ {
        Inline{&year_}
    };

    SyntaxRule yearEnd_ {
        Inline{&year_}
    };

    SyntaxRule holder_ {
        Repeat{2, 3,
            Sequence{
                Not{"All"},
                &word_,
                Repeat{0, 1, '.'},
                Repeat{0, 2, ' '}
            }
        }
    };

    SyntaxRule description_ {
        Repeat{
            Sequence{
                Not{Inline{&copyright_}},
                Any{}
            }
        }
    };

    SyntaxRule copyright_ {
        Sequence{
            Repeat{OneOf{" \t"}},
            "Copyright",
            Repeat{OneOf{" \t"}},
            Repeat{0, 1,
                Choice{
                    "(C)",
                    "(c)",
                    "©"
                }
            },
            Repeat{OneOf{" \t,"}},
            &yearStart_,
            Repeat{OneOf{" -"}},
            Repeat{0, 1, &yearEnd_},
            Repeat{OneOf{" \t,"}},
            &holder_,
            Repeat{OneOf{" \t."}},
            Repeat{Other{'\n'}},
            Char{'\n'}
        }
    };

    SyntaxRule statement_ {
        Repeat{1,
            Sequence{
                Not{
                    Sequence{
                        Repeat{OneOf{" \t\n\r"}},
                        Eoi{}
                    }
                },
                Any{}
            }
        }
    };

    SyntaxRule notice_ {
        Sequence{
            Repeat{OneOf{" \t\n\r"}},
            &description_,
            Repeat{1,
                Sequence{
                    &copyright_,
                    Repeat{OneOf{" \t\n\r"}}
                }
            },
            Repeat{OneOf{" \t\n\r"}},
            &statement_,
            Repeat{OneOf{" \t\n\r"}}
        }
    };

    State():
        SyntaxDefinition::State{&notice_}
    {}

    Notice readNotice(const Header &header) const
    {
        String message = header.message();
        Token rootToken = match(message);
        if (!rootToken) return Notice{};
        assert(rootToken.children().count() >= 2);
        auto it = rootToken.children().begin() + 1;
        List<Copyright> copyrights;
        for (; it; ++it) {
            Token token = *it;
            if (token.rule() != copyright_) break;
            copyrights.append(readCopyright(token, message));
        }
        assert(it->rule() == statement_);
        String statement = message.copy(*it);
        return Notice{copyrights, statement, header};
    }

    Copyright readCopyright(const Token &token, const String &message) const
    {
        assert(token.rule() == copyright_);
        auto it = token.children().begin();
        assert(token.rule() == yearStart_);
        int yearStart = message.copy(*it).toInt();
        int yearEnd = yearStart;
        ++it;
        assert(it);
        if (it->rule() == yearEnd_) {
            yearEnd = message.copy(*it).toInt();
            ++it;
        }
        assert(it);
        assert(it->rule() == holder_);
        it = it->children().begin();
        assert(*it);
        List<String> parts;
        for (; it; ++it) {
            assert(it->rule() == word_);
            parts.append(message.copy(*it));
        }
        String holder = parts.join(' ');
        return Copyright{holder, yearStart, yearEnd};
    }
};

NoticeSyntax::NoticeSyntax()
{
    initOnce<State>();
}

Notice NoticeSyntax::readNotice(const Header &header) const
{
    return me().readNotice(header);
}

const NoticeSyntax::State &NoticeSyntax::me() const
{
    return Object::me.as<State>();
}

} // namespace cc::claim
