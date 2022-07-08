/*
 * Copyright (C) 2007-2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/toki/CxxSyntax>
#include <cc/IntegerSyntax>
#include <cc/FloatSyntax>

namespace cc::toki {

struct CxxSyntax::State final: public HighlightingSyntax::State
{
    VoidRule integerLiteral_ { IntegerSyntax{} };
    VoidRule floatLiteral_ { FloatSyntax{} };

    SyntaxRule commentText_ {
        Sequence{
            "/*",
            Find{"*/"}
        }
    };

    SyntaxRule commentLine_ {
        Sequence{
            "//",
            Find{Ahead{'\n'}}
        }
    };

    SyntaxRule preprocessing_ {
        Sequence{
            Repeat{OneOf{" \t"}},
            '#',
            Repeat{
                Choice{
                    &commentText_,
                    &commentLine_,
                    "\\\n",
                    Other{'\n'}
                }
            }
        }
    };

    SyntaxRule escapedChar_ {
        Sequence{
            '\\',
            Choice{
                Repeat{1, 3, Within{'0', '9'}},
                Other{'\n'}
            }
        }
    };

    SyntaxRule text_ { "Text",
        Repeat{1,
            Sequence{
                '"',
                Repeat{
                    Choice{
                        &escapedChar_,
                        NoneOf{"\"\n"}
                    }
                },
                '"'
            }
        }
    };

    SyntaxRule objcEscapedChar_ {
        Inline{&escapedChar_}
    };

    SyntaxRule objcText_ {
        Sequence{
            '@',
            Repeat{1,
                Sequence{
                    Char{'"'},
                    Repeat{
                        Choice{
                            &objcEscapedChar_,
                            NoneOf{"\"\n"}
                        }
                    },
                    Char{'"'}
                }
            }
        }
    };

    SyntaxRule punctuator_ {
        Keyword{
            "{" , "}" , "("  , ")"  , "[" , "]"  , ":" , "...", ";",
            "<" , ">" , ","  , "*"  , "&" , "="  , "~" ,
            "##", "?" , "."  , ".*" ,
            "+" , "-" , "/"  , "%"  , "^" , "|"  , "!" ,
            "+=", "-=", "*=" , "/=" , "%=", "^=" , "&=", "|=",
            "<<", ">>", "<<=", ">>=", "==", "!=" , "<=", ">=",
            "&&", "||", "++" , "--" , "->", "->*"
        }
    };

    SyntaxRule name_ {
        Sequence{
            Choice{
                Within{'a', 'z'},
                Within{'A', 'Z'},
                '_'
            },
            Repeat{
                Choice{
                    Within{'a', 'z'},
                    Within{'A', 'Z'},
                    Within{'0', '9'},
                    '_'
                }
            }
        }
    };

    SyntaxRule keyword_ {
        Sequence{
            Keyword{
                "namespace", "using",
                "export", "template", "typename", "enum",
                "class", "union", "struct", "typedef", "friend", "operator",
                "public", "private", "protected",
                "virtual", "explicit", "override", "final",
                "inline", "throw",
                "auto", "register", "static", "extern", "mutable",
                "new", "delete", "new[]", "delete[]",
                "and", "and_eq", "asm", "bitand", "bitor",
                "break", "case", "catch",
                "compl", "const_cast", "continue", "default", "delete",
                "do", "dynamic_cast", "else",
                "false", "for",
                "goto", "if",
                "new", "not", "not_eq", "or",
                "or_eq", "reinterpret_cast",
                "return", "sizeof", "static_cast",
                "switch", "this", "true",
                "try", "typeid",
                "while",
                "xor", "xor_eq"
            },
            Not{Inline{&name_}}
        }
    };

    SyntaxRule typeKeyword_ {
        Sequence{
            Keyword{
                "const", "volatile",
                "void", "nullptr",
                "signed", "unsigned",
                "long", "short",
                "int", "bool", "float", "double",
                "char", "wchar_t",
                "uint8_t", "uint16_t", "uint32_t", "uint64_t",
                "int8_t", "int16_t", "int32_t", "int64_t",
                "float32_t", "float64_t",
                "size_t", "ssize_t", "off_t"
            },
            Not{Inline{&name_}}
        }
    };

    SyntaxRule qtKeyword_ {
        Sequence{
            Choice{
                Sequence{
                    "Q_",
                    Inline{&name_}
                },
                Keyword{
                    "SIGNAL", "SLOT",
                    "signals", "slots",
                    "emit", "tr", "qobject_cast", "foreach",
                    "qscriptvalue_cast"
                }
            },
            Not{Inline{&name_}}
        }
    };

    SyntaxRule objcKeyword_ {
        Sequence{
            Char{'@'},
            Keyword{
                "interface", "implementation", "protocol", "end",
                "private", "protected", "public",
                "try", "catch", "throw", "finally",
                "property", "synthesize", "dynamic",
                "class", "selector", "protocol", "encode", "synchronized"
            },
            Not{Inline{&name_}}
        }
    };

    SyntaxRule identifier_ {
        Repeat{1,
            Sequence{
                Repeat{0, 1, "::"},
                &name_
            }
        }
    };

    SyntaxRule integer_ {
        Sequence{
            &integerLiteral_,
            Repeat{0, 1, OneOf{"uU"}},
            Repeat{0, 2, OneOf{"lL"}},
            Not{OneOf{".eE"}}
        }
    };

    SyntaxRule float_ {
        Sequence{
            &floatLiteral_,
            Repeat{0, 1, OneOf{"fFlL"}}
        }
    };

    SyntaxRule char_ {
        Sequence{
            Char{'\''},
            Choice{
                &escapedChar_,
                Other{'\''}
            },
            Char{'\''}
        }
    };

    SyntaxRule source_ {
        Repeat{
            Choice{
                Repeat{1, OneOf{" \t"}},
                &commentText_,
                &commentLine_,
                Sequence{
                    Choice{
                        '\n',
                        Boi{}
                    },
                    &preprocessing_
                },
                '\n',
                &text_,
                &objcText_,
                &punctuator_,
                &keyword_,
                &typeKeyword_,
                &qtKeyword_,
                &objcKeyword_,
                &identifier_,
                &integer_,
                &float_,
                &char_,
                Any{}
            }
        }
    };

    State():
        HighlightingSyntax::State{&source_}
    {
        define("CommentLine", commentLine_);
        define("CommentText", commentText_);
        define("Preprocessing", preprocessing_);
        define("Text", text_);
        define("EscapedChar", escapedChar_);
        define("ObjcText", objcText_);
        define("ObjcEscapedChar", objcEscapedChar_);
        define("Keyword", keyword_);
        define("TypeKeyword", typeKeyword_);
        define("ObjcKeyword", objcKeyword_),
        define("Integer", integer_);
        define("Float", float_);
        define("Char", char_);
    }
};

CxxSyntax::CxxSyntax()
{
    initOnce<State>();
}

const SyntaxRule &CxxSyntax::commentLine() const
{
    return me().commentLine_;
}

const SyntaxRule &CxxSyntax::commentText() const
{
    return me().commentText_;
}

const CxxSyntax::State &CxxSyntax::me() const
{
    return Object::me.as<State>();
}

} // namespace cc::toki
