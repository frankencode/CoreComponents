/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Singleton>
#include <cc/Format>
#include <cc/syntax/exceptions>
#include <cc/syntax/FloatSyntax>
#include <cc/syntax/IntegerSyntax>
#include <cc/meta/YasonSyntax>

namespace cc {
namespace meta {

const YasonSyntax *YasonSyntax::instance()
{
    return Singleton<YasonSyntax>::instance();
}

YasonSyntax::YasonSyntax(int options)
{
    SYNTAX("yason");

    IMPORT(FloatSyntax::instance(), "float");
    IMPORT(IntegerSyntax::instance(), "int");

    DEFINE_OPTIONAL("CommentLine", options & GenerateComments,
        GLUE(
            STRING("//"),
            CHOICE(
                FIND(AHEAD(CHAR('\n'))),
                FIND(EOI())
            )
        )
    );

    DEFINE_OPTIONAL("CommentText", options & GenerateComments,
        GLUE(
            STRING("/*"),
            REPEAT(
                CHOICE(
                    REF("CommentText"),
                    GLUE(
                        NOT(STRING("*/")),
                        ANY()
                    )
                )
            ),
            HINT("missing closing '*/'",
                STRING("*/")
            )
        )
    );

    DEFINE_VOID("Noise",
        REPEAT(
            CHOICE(
                RANGE(" \t\n\r"),
                REF("CommentLine"),
                REF("CommentText")
            )
        )
    );

    DEFINE_OPTIONAL("EscapedChar", options & GenerateEscapedChars,
        GLUE(
            CHAR('\\'),
            EXPECT("illegal escape sequence",
                CHOICE(
                    RANGE("\"\\/fnrt"),
                    GLUE(
                        CHAR('u'),
                        REPEAT(4, 4,
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
    );

    DEFINE("Item", &item_,
        REPEAT(1,
            GLUE(
                NOT(
                    GLUE(
                        INLINE("Noise"),
                        RANGE(",]")
                    )
                ),
                EXCEPT(",;{}:\"\n\r")
            )
        )
    );

    DEFINE("Line", &line_,
        GLUE(
            NOT(RANGE("[{")),
            LENGTH(1,
                FIND(
                    AHEAD(
                        GLUE(
                            REPEAT(
                                CHOICE(
                                    RANGE(" \t\r}"),
                                    INLINE("CommentLine"),
                                    INLINE("CommentText")
                                )
                            ),
                            CHOICE(
                                AHEAD(CHAR('\n')),
                                EOI()
                            )
                        )
                    )
                )
            )
        )
    );

    DEFINE("String", &string_,
        GLUE(
            CHAR('"'),
            REPEAT(
                CHOICE(
                    REF("EscapedChar"),
                    EXCEPT("\"\n")
                )
            ),
            EXPECT("missing closing '\"'",
                CHAR('"')
            )
        )
    );

    DEFINE("Document", &document_,
        FIND(AHEAD(REPLAY("eod")))
    );

    DEFINE("Text", &text_,
        CHOICE(
            GLUE(
                REF("String"),
                REPEAT(
                    GLUE(
                        INLINE("Noise"),
                        REF("String")
                    )
                )
            ),
            GLUE(
                STRING("<<"),
                CAPTURE("eod", INLINE("Identifier")),
                REPEAT(RANGE(" \t")),
                REPEAT(0, 1, REF("CommentLine")),
                CHAR('\n'),
                REF("Document"),
                REPLAY("eod")
            ),
            CONTEXT("List",
                REF("Item"),
                REF("Line")
            )
        )
    );

    DEFINE("Boolean", &boolean_,
        KEYWORD("true false")
    );

    true_ = keywordByName("true");
    false_ = keywordByName("false");

    DEFINE("Integer", &integer_,
        INLINE("int::Literal")
    );

    DEFINE("Float", &float_,
        INLINE("float::Literal")
    );

    DEFINE_VOID("HexDigit",
        CHOICE(
            RANGE('0', '9'),
            RANGE('A', 'F'),
            RANGE('a', 'f')
        )
    );

    DEFINE("Color", &color_,
        GLUE(
            CHAR('#'),
            CHOICE(
                REPEAT(8, 8, INLINE("HexDigit")),
                REPEAT(6, 6, INLINE("HexDigit")),
                REPEAT(3, 3, INLINE("HexDigit"))
            )
        )
    );

    DEFINE("Version", &version_,
        GLUE(
            REPEAT(0, 1, CHAR('v')),
            REPEAT(1, 6, RANGE('0', '9')),
            CHAR('.'),
            REPEAT(1, 6, RANGE('0', '9')),
            REPEAT(0, 1,
                GLUE(
                    CHAR('.'),
                    REPEAT(1, 6, RANGE('0', '9'))
                )
            )
        )
    );

    DEFINE("Identifier",
        REPEAT(1,
            CHOICE(
                RANGE('a', 'z'),
                RANGE('A', 'Z'),
                RANGE("_-"),
                RANGE('0', '9')
            )
        )
    );

    DEFINE("Name", &name_,
        CHOICE(
            GLUE(
                NOT(RANGE('0', '9')),
                INLINE("Identifier")
            ),
            INLINE("String")
        )
    );

    DEFINE("ClassName", &className_,
        GLUE(
            AHEAD(RANGE('A', 'Z')),
            INLINE("Identifier")
        )
    );

    DEFINE("Member",
        CHOICE(
            REF("Object"),
            GLUE(
                REF("Name"),
                INLINE("Noise"),
                EXPECT("expected ':'",
                    CHAR(':')
                ),
                INLINE("Noise"),
                EXPECT("expected value",
                    INLINE("Value")
                )
            )
        )
    );

    DEFINE("Object", &object_,
        GLUE(
            REPEAT(0, 1,
                GLUE(
                    REF("ClassName"),
                    INLINE("Noise")
                )
            ),
            CHAR('{'),
            INLINE("Noise"),
            REPEAT(0, 1,
                GLUE(
                    INLINE("Member"),
                    REPEAT(
                        GLUE(
                            LENGTH(1,
                                GLUE(
                                    INLINE("Noise"),
                                    REPEAT(0, 1,
                                        GLUE(
                                            RANGE(",;"),
                                            INLINE("Noise")
                                        )
                                    )
                                )
                            ),
                            INLINE("Member")
                        )
                    )
                )
            ),
            EXPECT("missing closing '}'",
                GLUE(
                    INLINE("Noise"),
                    CHAR('}')
                )
            )
        )
    );

    DEFINE("List", &list_,
        GLUE(
            CHAR('['),
            INLINE("Noise"),
            REPEAT(0, 1,
                GLUE(
                    INLINE("Value"),
                    REPEAT(
                        GLUE(
                            INLINE("Noise"),
                            RANGE(",;"),
                            INLINE("Noise"),
                            INLINE("Value")
                        )
                    )
                )
            ),
            EXPECT("missing closing ']'",
                GLUE(
                    INLINE("Noise"),
                    CHAR(']')
                )
            )
        )
    );

    DEFINE("Value",
        CHOICE(
            REF("Integer"),
            REF("Float"),
            REF("Color"),
            REF("Version"),
            REF("Object"),
            REF("List"),
            REF("Boolean"),
            REF("Text")
        )
    );

    DEFINE("Message", &message_,
        GLUE(
            INLINE("Noise"),
            EXPECT("expected YASON expression",
                INLINE("Value")
            ),
            INLINE("Noise")
        )
    );

    ENTRY("Message");
    LINK();
}

Variant YasonSyntax::parse(const CharArray *text, const MetaProtocol *protocol) const
{
    Ref<SyntaxState> state = match(text);
    if (!state->valid()) throw SyntaxError{text, state};
    Token *valueToken = state->rootToken()->firstChild();
    if (protocol) return readObject(text, valueToken, protocol);
    return readValue(text, valueToken);
}

Ref<MetaObject> YasonSyntax::readObject(const CharArray *text, Token *token, const MetaProtocol *protocol, MetaObject *prototype) const
{
    if (token->rule() != object_)
        throw SemanticError{"Expected an object value", text, token->i0()};

    Token *objectToken = token;
    token = token->firstChild();

    String className;
    if (token && token->rule() == className_)
        className = text->copyRange(token);

    if (protocol) {
        if (!protocol->lookup(className, &prototype)) {
            throw SemanticError{
                Format{"Object of class \"%%\" is not allowed here"} << className,
                text, token->i1()
            };
        }
    }
    else if (prototype) {
        if (className != prototype->className()) {
            throw SemanticError{
                Format{"Expected an object of class \"%%\""} << prototype->className(),
                text, token->i1()
            };
        }
    }

    if (token && token->rule() == className_)
        token = token->nextSibling();

    Ref<MetaObject> object;
    if (prototype) {
        if (protocol) object = protocol->produce(prototype);
        else object = prototype->produce();
    }
    else object = MetaObject::create(className);

    while (token) {
        if (token->rule() == name_) {
            String name = readName(text, token);

            Variant defaultValue;
            MetaObject *memberPrototype = 0;
            if (prototype) {
                if (prototype->count() > 0) {
                    if (!prototype->lookup(name, &defaultValue)) {
                        throw SemanticError{
                            Format{"Member \"%%\" is not supported"} << name,
                            text, token->i1()
                        };
                    }
                    if (defaultValue->type() == VariantType::Object)
                        memberPrototype = Variant::cast<MetaObject *>(defaultValue);
                }
            }

            token = token->nextSibling();

            Variant value;
            if (memberPrototype)
                value = readObject(text, token, 0, memberPrototype);
            else
                value = readValue(text, token, defaultValue->type(), defaultValue->itemType());

            Variant existingValue;
            if (object->lookup(name, &existingValue)) {
                if (value != existingValue) {
                    throw SemanticError{
                        Format{"Ambiguous value for member \"%%\""} << name,
                        text, token->i1()
                    };
                }
            }
            object->insert(name, value);
        }
        else {
            const MetaProtocol *prototypeProtocol = 0;
            if (prototype) prototypeProtocol = prototype->protocol();
            if (prototypeProtocol) {
                if (object->children()->count() >= prototypeProtocol->maxCount()) {
                    throw SemanticError{
                        Format{"Maximum number of children (%%) exceeded"} << prototypeProtocol->maxCount(),
                        text, token->i0()
                    };
                }
            }
            Ref<MetaObject> child = readObject(text, token, prototypeProtocol);
            object->children()->append(child);
        }
        token = token->nextSibling();
    }

    object->autocomplete(prototype);

    if (prototype) {
        const MetaProtocol *prototypeProtocol = prototype->protocol();
        if (prototypeProtocol) {
            if (prototypeProtocol->minCount() > 0) {
                if (!object->hasChildren() || object->children()->count() < prototypeProtocol->minCount()) {
                    if (prototypeProtocol->prototypes_->count() == 1 && prototypeProtocol->minCount() == 1) {
                        throw SemanticError(
                            Format("Object of type %% needs at least one child of type %%") << object->className() << prototypeProtocol->prototypes_->at(0)->value()->className(),
                            text, objectToken->i0()
                        );
                    }
                    else {
                        throw SemanticError(
                            Format("Object of type %% needs at least %% children") << object->className() << prototypeProtocol->minCount(),
                            text, objectToken->i0()
                        );
                    }
                }
            }
        }
    }

    object->realize(text, objectToken);

    return object;
}

Token *YasonSyntax::nameToken(const CharArray *text, Token *objectToken, const String &memberName) const
{
    for (Token *token = objectToken->firstChild(); token; token = token->nextSibling()) {
        if (token->rule() == name_) {
            if (readName(text, token) == memberName)
                return token;
        }
    }
    return objectToken;
}

Token *YasonSyntax::valueToken(const CharArray *text, Token *objectToken, const String &memberName) const
{
    Token *token = nameToken(text, objectToken, memberName);
    if (token != objectToken) return token->nextSibling();
    return objectToken;
}

Token *YasonSyntax::childToken(Token *objectToken, int childIndex) const
{
    for (Token *token = objectToken->firstChild(); token; token = token->nextSibling()) {
        if (token->rule() == object_) {
            if (childIndex == 0) return token;
            --childIndex;
        }
    }
    return 0;
}

String YasonSyntax::readName(const CharArray *text, Token *token) const
{
    bool stripQuotation = (text->at(token->i0()) == '"');
    return text->copy(token->i0() + stripQuotation, token->i1() - stripQuotation);
}

Variant YasonSyntax::readValue(const CharArray *text, Token *token, VariantType expectedType, VariantType expectedItemType) const
{
    Variant value;
    bool typeError = false;

    if (token->rule() == float_)
    {
        if ( expectedType == VariantType::Undefined ||
             expectedType == VariantType::Float ||
             expectedItemType == VariantType::Float )
        {
            float64_t x;
            FloatSyntax::instance()->read(&x, text, token);
            value = x;
        }
        else
            typeError = true;
    }
    else if (token->rule() == integer_)
    {
        if ( expectedType == VariantType::Undefined ||
             expectedType == VariantType::Float ||
             expectedType == VariantType::Int ||
             expectedItemType == VariantType::Float ||
             expectedItemType == VariantType::Int )
        {
            uint64_t x; int s;
            IntegerSyntax::instance()->read(&x, &s, text, token);
            value = int(x) * s;
            if (expectedType == VariantType::Float)
                value = Variant::Float(value);
        }
        else
            typeError = true;
    }
    else if (token->rule() == color_)
    {
        if ( expectedType == VariantType::Undefined ||
             expectedType == VariantType::Color ||
             expectedItemType == VariantType::Color )
        {
            value = Color(String(text->copyRange(token)));
        }
        else
            typeError = true;
    }
    else if (token->rule() == version_)
    {
        if ( expectedType == VariantType::Undefined ||
             expectedType == VariantType::Version ||
             expectedType == VariantType::String ||
             expectedItemType == VariantType::Version ||
             expectedItemType == VariantType::String )
        {
            String chunk = text->copyRange(token);
            if ( expectedType == VariantType::String ||
                 expectedItemType == VariantType::String )
                value = chunk;
            else
                value = Version(chunk);
        }
        else
            typeError = true;
    }
    else if (token->rule() == text_)
    {
        if ( expectedType == VariantType::Undefined ||
             expectedType == VariantType::String ||
             expectedItemType == VariantType::String )
        {
            value = readText(text, token);
        }
        else if ( expectedType == VariantType::Color ||
                  expectedItemType == VariantType::Color )
        {
            String s = readText(text, token);
            bool ok = false;
            value = Color::parse(s, &ok);
            if (!ok) typeError = true;
        }
        else
            typeError = true;
    }
    else if (token->rule() == object_)
    {
        value = readObject(text, token);
    }
    else if (token->rule() == list_)
    {
        if ( expectedType == VariantType::Undefined ||
             expectedType == VariantType::List )
            value = readList(text, token, expectedItemType);
        else
            typeError = true;
    }
    else if (token->rule() == boolean_)
    {
        if ( expectedType == VariantType::Undefined ||
             expectedType == VariantType::Bool ||
             expectedItemType == VariantType::Bool )
        {
            if (token->keyword() == true_)
                value = true;
            else if (token->keyword() == false_)
                value = false;
        }
        else
            typeError = true;
    }

    if (typeError) {
        throw SemanticError(
            Format("Expected a value of type %%") << Variant::typeName(expectedType, expectedItemType),
            text, token->i0()
        );
    }

    if (expectedType == VariantType::List && value->type() != VariantType::List) {
        if (expectedItemType == VariantType::Int) {
            Ref< List<int> > list = List<int>::create();
            list->append(value);
            value = list;
        }
        else if (expectedItemType == VariantType::Bool) {
            Ref< List<bool> > list = List<bool>::create();
            list->append(value);
            value = list;
        }
        else if (expectedItemType == VariantType::Float) {
            Ref< List<float> > list = List<float>::create();
            list->append(value);
            value = list;
        }
        else if (expectedItemType == VariantType::String) {
            Ref< List<String> > list = List<String>::create();
            list->append(value);
            value = list;
        }
        else {
            Ref<VariantList> list = VariantList::create();
            list->append(value);
            value = list;
        }
    }

    return value;
}

Variant YasonSyntax::readList(const CharArray *text, Token *token, VariantType expectedItemType) const
{
    Variant list;
    if (expectedItemType == VariantType::Int)
        list = parseTypedList<int>(text, token, expectedItemType);
    else if (expectedItemType == VariantType::Bool)
        list = parseTypedList<bool>(text, token, expectedItemType);
    else if (expectedItemType == VariantType::Float)
        list = parseTypedList<float>(text, token, expectedItemType);
    else if (expectedItemType == VariantType::String)
        list = parseTypedList<String>(text, token, expectedItemType);
    else
        list = parseTypedList<Variant>(text, token, expectedItemType);
    return list;
}

String YasonSyntax::readText(const CharArray *text, Token *token) const
{
    token = token->firstChild();

    String s;

    if (token->rule() != string_) {
        s = text->copyRange(token);
    }
    else {
        Ref<StringList> l = StringList::create();
        while (token) {
            l->append(text->copy(token->i0() + 1, token->i1() - 1));
            token = token->nextSibling();
        }
        s = (l->count() == 1) ? l->at(0) : l->join();
        mutate(s)->unescapeInsitu();
    }

    return s;
}

}} // namespace cc::meta
