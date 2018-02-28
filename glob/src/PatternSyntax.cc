/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Singleton>
#include <cc/Format>
#ifndef NDEBUG
#include <cc/syntax/SyntaxDebugger>
#endif
#include <cc/syntax/syntax>
#include <cc/glob/Pattern>
#include <cc/glob/PatternSyntax>

namespace cc {
namespace glob {

const PatternSyntax *PatternSyntax::instance()
{
    return Singleton<PatternSyntax>::instance();
}

PatternSyntax::PatternSyntax()
{
    SYNTAX("pattern");

    any_ = DEFINE("Any", CHAR('#'));
    gap_ = DEFINE("Gap", CHAR('*'));
    boi_ = DEFINE("Boi", GLUE(BOI(), CHAR('^')));
    eoi_ = DEFINE("Eoi", GLUE(CHAR('^'), EOI()));

    char_ =
        DEFINE("Char",
            CHOICE(
                EXCEPT("#*\\[](){}|^:"),
                GLUE(
                    CHAR('\\'),
                    EXPECT("Invalid escape sequence",
                        CHOICE(
                            RANGE(
                                "#*\\[](){}|^:"
                                "nstrf"
                            ),
                            GLUE(
                                CHAR('x'),
                                REPEAT(2, 2,
                                    CHOICE(
                                        RANGE('0', '9'),
                                        RANGE('a', 'f'),
                                        RANGE('A', 'F')
                                    )
                                )
                            )
                        )
                    )
                )
            )
        );

    string_ =
        DEFINE("String",
            REPEAT(2,
                REF("Char")
            )
        );

    rangeMinMax_ =
        DEFINE("RangeMinMax",
            GLUE(
                CHAR('['),
                CHOICE(
                    GLUE(
                        REPEAT(0, 1, CHAR('^')),
                        CHOICE(
                            GLUE(
                                STRING(".."),
                                REF("Char")
                            ),
                            GLUE(
                                REF("Char"),
                                STRING(".."),
                                REF("Char")
                            ),
                            GLUE(
                                REF("Char"),
                                STRING("..")
                            )
                        )
                    ),
                    STRING("..")
                ),
                EXPECT("Expected closing ']'",
                    CHAR(']')
                )
            )
        );

    rangeExplicit_ =
        DEFINE("RangeExplicit",
            GLUE(
                CHAR('['),
                REPEAT(0, 1, CHAR('^')),
                REPEAT(1, REF("Char")),
                EXPECT("Expected closing ']'",
                    CHAR(']')
                )
            )
        );

    DEFINE("Range",
        GLUE(
            AHEAD(CHAR('[')),
            EXPECT("Expected range definition",
                CHOICE(
                    REF("RangeMinMax"),
                    REF("RangeExplicit")
                )
            )
        )
    );

    DEFINE("Number",
        REPEAT(1, 20,
            RANGE('0', '9')
        )
    );

    minRepeat_ = DEFINE("MinRepeat", INLINE("Number"));
    maxRepeat_ = DEFINE("MaxRepeat", INLINE("Number"));

    repeat_ =
        DEFINE("Repeat",
            GLUE(
                CHAR('{'),
                REPEAT(0, 1, RANGE("<~>")),
                REPEAT(0, 1,
                    GLUE(
                        REPEAT(0, 1, REF("MinRepeat")),
                        STRING(".."),
                        REPEAT(0, 1, REF("MaxRepeat")),
                        EXPECT("Expected ':' after repeat counts",
                            CHAR(':')
                        )
                    )
                ),
                EXPECT("Expected repeat expression",
                    REF("Choice")
                ),
                EXPECT("Expected closing '}'",
                    CHAR('}')
                )
            )
        );

    sequence_ =
        DEFINE("Sequence",
            REPEAT(
                CHOICE(
                    REF("Repeat"),
                    REF("String"),
                    REF("Char"),
                    REF("Any"),
                    REF("Gap"),
                    INLINE("Range"),
                    REF("Boi"),
                    REF("Eoi"),
                    REF("Group"),
                    REF("Behind"),
                    REF("Ahead"),
                    REF("Capture"),
                    REF("Replay")
                )
            )
        );

    group_ =
        DEFINE("Group",
            GLUE(
                CHAR('('),
                NOT(CHAR('?')),
                REF("Choice"),
                EXPECT("Expected closing ')'",
                    CHAR(')')
                )
            )
        );

    ahead_ =
        DEFINE("Ahead",
            GLUE(
                STRING("(?>"),
                REPEAT(0, 1, CHAR('!')),
                REPEAT(0, 1, CHAR(':')),
                EXPECT("Expected ahead expression",
                    REF("Choice")
                ),
                EXPECT("Expected closing ')'",
                    CHAR(')')
                )
            )
        );

    behind_ =
        DEFINE("Behind",
            GLUE(
                STRING("(?<"),
                REPEAT(0, 1, CHAR('!')),
                REPEAT(0, 1, CHAR(':')),
                EXPECT("Expected behind expression",
                    REF("Choice")
                ),
                EXPECT("Expected closing ')'",
                    CHAR(')')
                )
            )
        );

    identifier_ =
        DEFINE("CaptureIdentifier",
            REPEAT(
                CHOICE(
                    RANGE('a', 'z'),
                    RANGE('A', 'Z'),
                    RANGE('0', '9'),
                    CHAR('_')
                )
            )
        );

    capture_ =
        DEFINE("Capture",
            GLUE(
                STRING("(?@"),
                REPEAT(0, 1,
                    GLUE(
                        REF("CaptureIdentifier"),
                        CHAR(':')
                    )
                ),
                EXPECT("Expected capture expression",
                    LENGTH(1,
                        REF("Choice")
                    )
                ),
                EXPECT("Expected closing ')'",
                    CHAR(')')
                )
            )
        );

    replay_ =
        DEFINE("Replay",
            GLUE(
                STRING("(?="),
                REF("CaptureIdentifier"),
                EXPECT("Expected closing ')'",
                    CHAR(')')
                )
            )
        );

    choice_ =
        DEFINE("Choice",
            GLUE(
                REF("Sequence"),
                REPEAT(
                    GLUE(
                        CHAR('|'),
                        REF("Sequence")
                    )
                )
            )
        );

    pattern_ =
        DEFINE("Pattern",
            GLUE(
                REF("Choice"),
                EOI()
            )
        );

    ENTRY("Pattern");
    LINK();
}

void PatternSyntax::compile(const CharArray *text, SyntaxDefinition *definition) const
{
    Ref<SyntaxState> state = match(text);
    if (!state->valid()) throw SyntaxError(text, state);
    NODE entry;
    if (text->count() == 0) entry = definition->PASS();
    else entry = compileChoice(text, state->rootToken()->firstChild(), definition);
    definition->DEFINE("Expression", entry);
    definition->ENTRY("Expression");
    definition->LINK();
}

NODE PatternSyntax::compileChoice(const CharArray *text, Token *token, SyntaxDefinition *definition) const
{
    if (token->countChildren() == 1)
        return compileSequence(text, token->firstChild(), definition);
    NODE node = new LazyChoiceNode;
    for (Token *child = token->firstChild(); child; child = child->nextSibling())
        node->appendChild(compileSequence(text, child, definition));
    return definition->debug(node, "Choice");
}

NODE PatternSyntax::compileSequence(const CharArray *text, Token *token, SyntaxDefinition *definition) const
{
    NODE node = new GlueNode;
    for (Token *child = token->firstChild(); child; child = child->nextSibling()) {
        if (child->rule() == string_) node->appendChild(definition->STRING(readString(text, child)));
        else if (child->rule() == char_) node->appendChild(definition->CHAR(readChar(text, child)));
        else if (child->rule() == any_) node->appendChild(definition->ANY());
        else if (child->rule() == gap_) node->appendChild(definition->GREEDY_REPEAT(definition->ANY()));
        else if (child->rule() == rangeMinMax_) node->appendChild(compileRangeMinMax(text, child, definition));
        else if (child->rule() == rangeExplicit_) node->appendChild(compileRangeExplicit(text, child, definition));
        else if (child->rule() == repeat_) { node->appendChild(compileRepeat(text, child, definition)); }
        else if (child->rule() == boi_) node->appendChild(definition->BOI());
        else if (child->rule() == eoi_) node->appendChild(definition->EOI());
        else if (child->rule() == group_) node->appendChild(compileChoice(text, child->firstChild(), definition));
        else if (child->rule() == ahead_) node->appendChild(compileAhead(text, child, definition));
        else if (child->rule() == behind_) node->appendChild(compileBehind(text, child, definition));
        else if (child->rule() == capture_) node->appendChild(compileCapture(text, child, definition));
        else if (child->rule() == replay_) node->appendChild(compileReference(text, child, definition));
    }
    if (node->firstChild() == node->lastChild()) {
        NODE child = node->firstChild();
        if (child) {
            child->unlink();
            return child;
        }
    }
    return definition->debug(node, "Glue");
}

NODE PatternSyntax::compileAhead(const CharArray *text, Token *token, SyntaxDefinition *definition) const
{
    return (text->at(token->i0() + 3) != '!') ?
        definition->AHEAD(compileChoice(text, token->firstChild(), definition)) :
        definition->NOT(compileChoice(text, token->firstChild(), definition));
}

NODE PatternSyntax::compileBehind(const CharArray *text, Token *token, SyntaxDefinition *definition) const
{
    return (text->at(token->i0() + 3) != '!') ?
        definition->BEHIND(compileChoice(text, token->firstChild(), definition)) :
        definition->NOT_BEHIND(compileChoice(text, token->firstChild(), definition));
}

NODE PatternSyntax::compileCapture(const CharArray *text, Token *token, SyntaxDefinition *definition) const
{
    String name;
    if (token->firstChild() != token->lastChild())
        name = text->copyRange(token->firstChild());
    return definition->CAPTURE(name, compileChoice(text, token->lastChild(), definition));
}

NODE PatternSyntax::compileReference(const CharArray *text, Token *token, SyntaxDefinition *definition) const
{
    String name = text->copyRange(token->firstChild());
    return definition->REPLAY(name);
}

char PatternSyntax::readChar(const CharArray *text, Token *token) const
{
    if (token->i1() - token->i0() > 1) {
        String h = text->copyRange(token);
        mutate(h)->unescapeInsitu();
        return h->at(0);
    }
    return text->at(token->i0());
}

String PatternSyntax::readString(const CharArray *text, Token *token) const
{
    String s(token->countChildren());
    int i = 0;
    for (Token *child = token->firstChild(); child; child = child->nextSibling())
        mutate(s)->at(i++) = readChar(text, child);
    return s;
}

NODE PatternSyntax::compileRangeMinMax(const CharArray *text, Token *token, SyntaxDefinition *definition) const
{
    int n = token->countChildren();
    bool invert = (text->at(token->i0() + 1) == '^');
    if (n == 2) {
        Token *min = token->firstChild();
        Token *max = min->nextSibling();
        char a = readChar(text, min);
        char b = readChar(text, max);
        return  invert ? definition->EXCEPT(a, b) : definition->RANGE(a, b);
    }
    else if (n == 1) {
        Token *child = token->firstChild();
        char ch = readChar(text, child);
        return invert ?
            ( (child->i0() - token->i0() <= 2) ? definition->BELOW(ch)            : definition->GREATER(ch)        ) :
            ( (child->i0() - token->i0() <= 2) ? definition->GREATER_OR_EQUAL(ch) : definition->BELOW_OR_EQUAL(ch) );
    }
    return definition->ANY();
}

NODE PatternSyntax::compileRangeExplicit(const CharArray *text, Token *token, SyntaxDefinition *definition) const
{
    Token *child = token->firstChild();
    bool invert = (text->at(token->i0() + 1) == '^');
    int n = token->countChildren();
    String s(n);
    for (int i = 0; i < n; ++i) {
        mutate(s)->at(i) = readChar(text, child);
        child = child->nextSibling();
    }
    return invert ? definition->EXCEPT(s) : definition->RANGE(s);
}

NODE PatternSyntax::compileRepeat(const CharArray *text, Token *token, SyntaxDefinition *definition) const
{
    Token *child = token->firstChild(), *min = 0, *max = 0;
    while (child) {
        if (child->rule() == minRepeat_) min = child;
        else if (child->rule() == maxRepeat_) max = child;
        child = child->nextSibling();
    }
    int minRepeat = min ? text->copyRange(min)->toNumber<int>() : 0;
    int maxRepeat = max ? text->copyRange(max)->toNumber<int>() : intMax;
    char modifier = text->at(token->i0() + 1);
    NODE node = compileChoice(text, token->lastChild(), definition);
    if (modifier == '<')
        return definition->LAZY_REPEAT(minRepeat, node);
    else if (modifier == '~')
        return definition->REPEAT(minRepeat, maxRepeat, node);
    // else if (modifier == '>');
    return definition->GREEDY_REPEAT(minRepeat, maxRepeat, node);
}

}} // namespace cc::glob
