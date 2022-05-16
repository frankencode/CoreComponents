/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/PatternSyntax>
#include <cc/Array>
#include <cc/Queue>
#include <cc/input>

namespace cc {

struct PatternSyntax::State: public SyntaxDefinition::State
{
    SyntaxRule any { '#' };

    SyntaxRule gap { '*' };

    SyntaxRule boi { Sequence { Boi{}, '^' } };

    SyntaxRule eoi { Sequence { '^', Eoi{} } };

    SyntaxRule character {
        Choice {
            NoneOf{'#', '*', '\\', '[', ']', '(', ')', '{', '}', '|', '^'},
            Sequence{
                '\\',
                Expect{
                    Choice{
                        OneOf{
                            '#', '*', '\\', '[', ']', '(', ')', '{', '}', '|', '^', ':',
                            'n', 's', 't', 'r', 'f'
                        },
                        Sequence{
                            'x',
                            Repeat{2, 2,
                                Choice{
                                    Within{'0', '9'},
                                    Within{'a', 'f'},
                                    Within{'A', 'F'}
                                }
                            }
                        }
                    },
                    "Invalid escape sequence"
                }
            }
        }
    };

    SyntaxRule string {  Repeat{2, &character} };

    SyntaxRule rangeMinMax {
        Sequence {
            '[',
            Choice{
                Sequence{
                    Repeat{0, 1, '^'},
                    Choice{
                        Sequence{ "..", &character },
                        Sequence{ &character, "..", &character },
                        Sequence{ &character, ".." }
                    }
                },
                ".."
            },
            Expect{']'}
        }
    };

    SyntaxRule rangeExplicit {
        Sequence{
            '[',
            Repeat{0, 1, '^'},
            Repeat{1, &character},
            Expect{']'}
        }
    };

    SyntaxRule range {
        Sequence{
            Ahead{'['},
            Expect{
                Choice{
                    &rangeMinMax,
                    &rangeExplicit
                },
                "Expected range definition"
            }
        }
    };

    SyntaxRule number {
        Repeat{1, 20,
            Within{'0', '9'}
        }
    };

    SyntaxRule minRepeat { Inline{&number} };
    SyntaxRule maxRepeat { Inline{&number} };

    SyntaxRule repeat {
        Sequence {
            '{',
            Repeat{0, 1,
                Sequence{
                    Repeat{0, 1, &minRepeat},
                    "..",
                    Repeat{0, 1, &maxRepeat},
                    Expect{':'}
                }
            },
            Expect{
                &choice,
                "Expected repeat expression"
            },
            Expect{'}'}
        }
    };

    SyntaxRule sequence {
        Repeat{
            Choice{
                &repeat,
                &string,
                &character,
                &any,
                &gap,
                Inline{&range},
                &boi,
                &eoi,
                &group,
                &behind,
                &ahead
            }
        }
    };

    SyntaxRule group {
        Sequence{
            '(',
            Not{OneOf{'>', '<', '^'}},
            &choice,
            Expect{')'}
        }
    };

    SyntaxRule ahead {
        Sequence{
            '(',
            Repeat{0, 1, '^'},
            '>',
            Expect{
                &choice,
                "Expected ahead expression"
            },
            Expect{')'}
        }
    };

    SyntaxRule behind {
        Sequence{
            '(',
            Repeat{0, 1, '^'},
            '<',
            Expect{
                &choice,
                "Expected begin expression"
            },
            Expect{')'}
        }
    };

    SyntaxRule choice {
        Sequence{
            &sequence,
            Repeat{
                Sequence{
                    '|',
                    &sequence
                }
            }
        }
    };

    SyntaxRule pattern {
        Sequence{
            &choice,
            Eoi{}
        }
    };

    State():
        SyntaxDefinition::State{&pattern}
    {}

    SyntaxNode compile(const String &text) const
    {
        if (text.count() == 0) return Pass{};

        Token token = match(text);
        if (!token) throw TextError{text, text.count(), "Illegal expression syntax"};

        return compileChoice(text, token.children().first());
    }

    SyntaxNode compileSequence(const String &text, const Token &token) const
    {
        return compileSequence(text, token, token.children().begin());
    }

    SyntaxNode compileSequence(const String &text, const Token &token, Token::Children::iterator head) const
    {
        Queue<SyntaxNode> nodes;

        for (auto it = head; it; ++it) {
            const Token &child = *it;
            if (child.rule() == string) nodes.pushBack(Literal{readString(text, child)});
            else if (child.rule() == character) nodes.pushBack(Char{readChar(text, child)});
            else if (child.rule() == any) nodes.pushBack(Any{});
            else if (child.rule() == gap) {
                auto succ = it + 1;
                if (succ) {
                    nodes.pushBack(FindLast{compileSequence(text, token, succ)});
                    break;
                }
                else nodes.pushBack(Repeat{Any{}});
            }
            else if (child.rule() == rangeMinMax) nodes.pushBack(compileRangeMinMax(text, child));
            else if (child.rule() == rangeExplicit) nodes.pushBack(compileRangeExplicit(text, child));
            else if (child.rule() == repeat) nodes.pushBack(compileRepeat(text, child));
            else if (child.rule() == boi) nodes.pushBack(Boi{});
            else if (child.rule() == eoi) nodes.pushBack(Eoi{});
            else if (child.rule() == group) nodes.pushBack(compileGroup(text, child.children().first()));
            else if (child.rule() == ahead) nodes.pushBack(compileAhead(text, child));
            else if (child.rule() == behind) nodes.pushBack(compileBehind(text, child));
        }

        if (nodes.count() == 1) return nodes.first();

        return Sequence{nodes.toArray<SyntaxNode>()};
    }

    SyntaxRule compileGroup(const String &text, const Token &token) const
    {
        return SyntaxRule{compileChoice(text, token)};
    }

    SyntaxNode compileChoice(const String &text, const Token &token) const
    {
        if (token.children().count() == 1)
            return compileSequence(text, token.children().first());

        Array<SyntaxNode> choices = Array<SyntaxNode>::allocate(token.children().count());
        long i = 0;
        for (const Token &child: token.children()) {
            choices[i++] = compileSequence(text, child);
        }

        return LongestChoice{choices};
    }

    SyntaxNode compileAhead(const String &text, const Token &token) const
    {
        return (text.at(token.i0() + 1) != '^') ?
            Ahead{compileChoice(text, token.children().first())}.as<SyntaxNode>() :
            Not{compileChoice(text, token.children().first())}.as<SyntaxNode>();
    }

    SyntaxNode compileBehind(const String &text, const Token &token) const
    {
        return (text.at(token.i0() + 1) != '^') ?
            Behind{compileChoice(text, token.children().first())}.as<SyntaxNode>() :
            NotBehind{compileChoice(text, token.children().first())}.as<SyntaxNode>();
    }

    char readChar(const String &text, const Token &token) const
    {
        if (token.i1() - token.i0() > 1) {
            String h = text.copy(token);
            h.expand();
            return h.at(0);
        }
        return text.at(token.i0());
    }

    String readString(const String &text, const Token &token) const
    {
        String s = String::allocate(token.children().count());
        long i = 0;
        for (const Token &child: token.children())
            s[i++] = readChar(text, child);
        return s;
    }

    SyntaxNode compileRangeMinMax(const String &text, const Token &token) const
    {
        bool invert = (text.at(token.i0() + 1) == '^');
        long n = token.children().count();
        if (n == 2) {
            char a = readChar(text, token.children().first());
            char b = readChar(text, token.children().last());
            return invert ? Outside{a, b}.as<SyntaxNode>() : Within{a, b}.as<SyntaxNode>();
        }
        else if (n == 1) {
            Token child = token.children().first();
            char ch = readChar(text, child);
            if (invert) {
                if (child.i0() - token.i0() <= 2)
                    return Match{[ch](char x) { return x <  ch; }};
                else
                    return Match{[ch](char x) { return x >  ch; }};
            }
            else {
                if (child.i0() - token.i0() <= 2)
                    return Match{[ch](char x) { return x >= ch; }};
                else
                    return Match{[ch](char x) { return x <= ch; }};
            }
        }
        return Any{};
    }

    SyntaxNode compileRangeExplicit(const String &text, const Token &token) const
    {
        bool invert = (text.at(token.i0() + 1) == '^');

        long i = 0, n = token.children().count();
        String s = String::allocate(n);
        for (const Token &child: token.children())
            s[i++] = readChar(text, child);

        return invert ? NoneOf{s}.as<SyntaxNode>() : OneOf{s}.as<SyntaxNode>();
    }

    SyntaxNode compileRepeat(const String &text, const Token &token) const
    {
        long minRepeatCount = 0;
        long maxRepeatCount = std::numeric_limits<long>::max();

        for (const Token &child: token.children()) {
            if (child.rule() == minRepeat) minRepeatCount = readNumber<long>(text.copy(child));
            else if (child.rule() == maxRepeat) maxRepeatCount = readNumber<long>(text.copy(child));
            else break;
        }

        return Repeat{
            minRepeatCount, maxRepeatCount,
            compileChoice(text, token.children().last())
        };
    }
};

PatternSyntax::PatternSyntax()
{
    initOnce<State>();
}

SyntaxRule PatternSyntax::compile(const String &text) const
{
    return SyntaxRule{me().compile(text)};
}

const PatternSyntax::State &PatternSyntax::me() const
{
    return Object::me.as<State>();
}

} // namespace cc
