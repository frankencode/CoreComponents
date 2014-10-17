/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/abnf/AbnfSyntax>

namespace flux {
namespace abnf {

AbnfSyntax::AbnfSyntax()
{
    SYNTAX("abnf");

    DEFINE_VOID("comment",
        GLUE(
            CHAR(';'),
            REPEAT(
                CHOICE(
                    INLINE("WSP"),
                    INLINE("VCHAR")
                )
            ),
            INLINE("CRLF")
        )
    );

    DEFINE_VOID("c-nl",
        CHOICE(
            INLINE("comment"),
            INLINE("CRLF")
        )
    );

    DEFINE_VOID("c-wsp",
        GLUE(
            REPEAT(0, 1, INLINE("c-nl")),
            INLINE("WSP")
        )
    );

    numVal_ =
        DEFINE("num-val",
            GLUE(
                CHAR('%'),
                CHOICE(
                    GLUE(RANGE("bB"), defineValue("BIT")),
                    GLUE(RANGE("dD"), defineValue("DIGIT")),
                    GLUE(RANGE("xX"), defineValue("HEXDIG"))
                )
            )
        );

    charVal_ =
        DEFINE("char-val",
            GLUE(
                CHAR('"'),
                REPEAT(OTHER('"')), // deviation from RFC5234
                CHAR('"')
            )
        );

    proseVal_ =
        DEFINE("prose-val",
            GLUE(
                CHAR('<'),
                REPEAT(OTHER('>')), // deviation from RFC5234
                CHAR('>')
            )
        );

    group_ =
        DEFINE("group",
            GLUE(
                CHAR('('),
                REPEAT(INLINE("c-wsp")),
                REF("alternation"),
                REPEAT(INLINE("c-wsp")),
                CHAR(')')
            )
        );

    option_ =
        DEFINE("option",
            GLUE(
                CHAR('['),
                REPEAT(INLINE("c-wsp")),
                REF("alternation"),
                REPEAT(INLINE("c-wsp")),
                CHAR(']')
            )
        );

    element_ =
        DEFINE("element",
            CHOICE(
                REF("rulename"),
                REF("group"),
                // REF("option"), // correction over RFC5234
                REF("num-val"),
                REF("char-val"),
                REF("prose-val")
            )
        );

    repeat_ =
        DEFINE("repeat",
            CHOICE(
                GLUE(
                    REPEAT(INLINE("DIGIT")),
                    CHAR('*'),
                    REPEAT(INLINE("DIGIT"))
                ),
                REPEAT(1, INLINE("DIGIT"))
            )
        );

    repetition_ =
        DEFINE("repetition",
            CHOICE(
                GLUE(
                    REPEAT(0, 1, REF("repeat")),
                    REF("element")
                ),
                REF("option")
            )
        );

    concatenation_ =
        DEFINE("concatenation",
            GLUE(
                REF("repetition"),
                REPEAT(
                    GLUE(
                        REPEAT(1, INLINE("c-wsp")),
                        REF("repetition")
                    )
                )
            )
        );

    alternation_ =
        DEFINE("alternation",
            GLUE(
                REF("concatenation"),
                REPEAT(
                    GLUE(
                        REPEAT(INLINE("c-wsp")),
                        CHAR('/'),
                        REPEAT(INLINE("c-wsp")),
                        REF("concatenation")
                    )
                )
            )
        );

    ruleName_ =
        DEFINE("rulename",
            GLUE(
                INLINE("ALPHA"),
                REPEAT(
                    CHOICE(
                        INLINE("ALPHA"),
                        INLINE("DIGIT"),
                        CHAR('-')
                    )
                )
            )
        );

    definedAs_ =
        DEFINE("defined-as",
            KEYWORD("= ~")
                // deviation from RFC5234: no support for redefinition ("=/")
                // and added differentiation between matching ('~') and production rules ('=')
        );

    rule_ =
        DEFINE("rule",
            GLUE(
                REF("rulename"),
                REPEAT(INLINE("c-wsp")),
                REF("defined-as"),
                REPEAT(INLINE("c-wsp")),
                REF("alternation"),
                    // correction of RFC5234, redundant rule "elements" substituted
                REPEAT(INLINE("c-wsp")),
                INLINE("c-nl")
            )
        );

    rulelist_ =
        DEFINE("rulelist",
            REPEAT(1,
                GLUE(
                    NOT(EOI()),
                    CHOICE(
                        REF("rule"),
                        GLUE(
                            REPEAT(INLINE("c-wsp")),
                            INLINE("c-nl")
                        ),
                        ERROR()
                    )
                )
            )
        );

    ENTRY("rulelist");
    LINK();
}

AbnfSyntax::NODE AbnfSyntax::defineValue(const char *digitRule)
{
    return
        GLUE(
            REPEAT(1, INLINE(digitRule)),
            REPEAT(0, 1,
                CHOICE(
                    REPEAT(1,
                        GLUE(
                            CHAR('.'),
                            REPEAT(1, INLINE(digitRule))
                        )
                    ),
                    GLUE(
                        CHAR('-'),
                        REPEAT(1, INLINE(digitRule))
                    )
                )
            )
        );
}

}} // namespace flux::abnf
