/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Singleton>
#include <cc/syntax/IntegerSyntax>
#include <cc/syntax/FloatSyntax>
#include <cc/toki/CxxSyntax>

namespace cc {
namespace toki {

const CxxSyntax *CxxSyntax::instance()
{
    return Singleton<CxxSyntax>::instance();
}

CxxSyntax::CxxSyntax()
{
    SYNTAX("cxx");

    IMPORT(IntegerSyntax::instance(), "int");
    IMPORT(FloatSyntax::instance(), "float");

    DEFINE("CommentText",
        GLUE(
            STRING("/*"),
            FIND(STRING("*/"))
        )
    );

    DEFINE("CommentLine",
        GLUE(
            STRING("//"),
            FIND(AHEAD(CHAR('\n')))
        )
    );

    DEFINE("Preprocessing",
        GLUE(
            REPEAT(RANGE(" \t")),
            CHAR('#'),
            REPEAT(
                CHOICE(
                    REF("CommentText"),
                    REF("CommentLine"),
                    STRING("\\\n"),
                    OTHER('\n')
                )
            )
        )
    );

    DEFINE("EscapedChar",
        GLUE(
            CHAR('\\'),
            CHOICE(
                REPEAT(1, 3, RANGE('0', '9')),
                OTHER('\n')
            )
        )
    );

    DEFINE("Text",
        REPEAT(1,
            GLUE(
                CHAR('"'),
                REPEAT(
                    CHOICE(
                        REF("EscapedChar"),
                        EXCEPT("\"\n")
                    )
                ),
                CHAR('"')
            )
        )
    );

    DEFINE("ObjcEscapedChar",
        INLINE("EscapedChar")
    );

    DEFINE("ObjcText",
        GLUE(
            CHAR('@'),
            REPEAT(1,
                GLUE(
                    CHAR('"'),
                    REPEAT(
                        CHOICE(
                            REF("ObjcEscapedChar"),
                            EXCEPT("\"\n")
                        )
                    ),
                    CHAR('"')
                )
            )
        )
    );

    DEFINE("Punctuator",
        KEYWORD(
            "{    }    (    )    [    ]    :   ... ; "
            "<    >    ,    *    &    =    ~         "
            "##   ?    .    .*                       "
            "+    -    /    %    ^    |    !         "
            "+=   -=   *=   /=   %=   ^=   &=  |=    "
            "<<   >>   <<=  >>=  ==   !=   <=  >=    "
            "&&   ||   ++   --   ->   ->*            "
        )
    );

    DEFINE("Name",
        GLUE(
            CHOICE(
                RANGE('a', 'z'),
                RANGE('A', 'Z'),
                CHAR('_')
            ),
            REPEAT(
                CHOICE(
                    RANGE('a', 'z'),
                    RANGE('A', 'Z'),
                    RANGE('0', '9'),
                    CHAR('_')
                )
            )
        )
    );

    DEFINE("Keyword",
        GLUE(
            KEYWORD(
                "namespace using                            "
                "export template typename enum              "
                "class union struct typedef friend operator "
                "public private protected                   "
                "virtual explicit override final            "
                "inline throw                               "
                "auto register static extern mutable        "
                "new delete new[] delete[]                  "
                "and and_eq asm bitand bitor                "
                "break case catch                           "
                "compl const_cast continue default delete   "
                "do dynamic_cast else                       "
                "false for                                  "
                "goto if                                    "
                "new not not_eq or                          "
                "or_eq reinterpret_cast                     "
                "return sizeof static_cast                  "
                "switch this true                           "
                "try typeid                                 "
                "while                                      "
                "xor xor_eq                                 "
            ),
            NOT(INLINE("Name"))
        )
    );

    DEFINE("TypeKeyword",
        GLUE(
            KEYWORD(
                "const volatile                     "
                "void nullptr                       "
                "signed unsigned                    "
                "long short                         "
                "int bool float double              "
                "char wchar_t                       "
                "uint8_t uint16_t uint32_t uint64_t "
                "int8_t int16_t int32_t int64_t     "
                "float32_t float64_t                "
                "size_t ssize_t off_t               "
            ),
            NOT(INLINE("Name"))
        )
    );

    DEFINE("QtKeyword",
        GLUE(
            CHOICE(
                GLUE(
                    STRING("Q_"),
                    INLINE("Name")
                ),
                KEYWORD(
                    "SIGNAL SLOT                      "
                    "signals slots connect disconnect "
                    "emit tr qobject_cast foreach     "
                    "qscriptvalue_cast                "
                )
            ),
            NOT(INLINE("Name"))
        )
    );

    DEFINE("ObjcKeyword",
        GLUE(
            CHAR('@'),
            KEYWORD(
                "interface implementation protocol end       "
                "private protected public                    "
                "try catch throw finally                     "
                "property synthesize dynamic                 "
                "class selector protocol encode synchronized "
            ),
            NOT(INLINE("Name"))
        )
    );

    DEFINE("Identifier",
        REPEAT(1,
            GLUE(
                REPEAT(0, 1, STRING("::")),
                REF("Name")
            )
        )
    );

    DEFINE("Integer",
        GLUE(
            INLINE("int::Literal"),
            REPEAT(0, 1, RANGE("uU")),
            REPEAT(0, 2, RANGE("lL"))
        )
    );

    DEFINE("Float",
        GLUE(
            INLINE("float::Literal"),
            REPEAT(0, 1, RANGE("fFlL"))
        )
    );

    DEFINE("Char",
        GLUE(
            CHAR('\''),
            CHOICE(
                REF("EscapedChar"),
                OTHER('\'')
            ),
            CHAR('\'')
        )
    );

    DEFINE("Code",
        REPEAT(
            CHOICE(
                REPEAT(1, RANGE(" \t")),
                REF("CommentText"),
                REF("CommentLine"),
                GLUE(
                    CHOICE(
                        CHAR('\n'),
                        BOI()
                    ),
                    REF("Preprocessing")
                ),
                CHAR('\n'),
                REF("Text"),
                REF("ObjcText"),
                REF("Punctuator"),
                REF("Keyword"),
                REF("TypeKeyword"),
                REF("QtKeyword"),
                REF("ObjcKeyword"),
                REF("Identifier"),
                REF("Float"),
                REF("Integer"),
                REF("Char"),
                ANY()
            )
        )
    );

    ENTRY("Code");
    LINK();
}

}} // namespace cc::toki
