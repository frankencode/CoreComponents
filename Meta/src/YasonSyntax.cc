/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/YasonSyntax>
#include <cc/MetaError>
#include <cc/FloatSyntax>
#include <cc/IntegerSyntax>
#include <cc/Color>
#include <cc/Version>
#include <cc/Format>

namespace cc {

struct YasonSyntax::State: public SyntaxDefinition::State
{
    FloatSyntax floatSyntax;
    IntegerSyntax integerSyntax;

    SyntaxRule real = floatSyntax;
    SyntaxRule integer = integerSyntax;

    SyntaxRule commentLine {
        Sequence {
            "//",
            Find{
                Choice{
                    Ahead{'\n'},
                    Eoi{}
                }
            }
        }
    };

    SyntaxRule commentText {
        Sequence {
            "/*",
            Expect{
                Find{"*/"},
                "Missing closing '*/'"
            }
        }
    };

    SyntaxRule noise {
        Repeat{
            Choice{
                OneOf{" \t\n\r"},
                Inline{&commentLine},
                Inline{&commentText}
            }
        }
    };

    SyntaxRule escapedChar {
        Sequence {
            '\\',
            Expect{
                Choice{
                    OneOf{"\"\\/fnrt"},
                    Sequence{
                        'u',
                        Repeat{4, 4, Inline{&hexDigit}}
                    }
                },
                "Illegal escape sequence"
            }
        }
    };

    SyntaxRule item {
        Repeat{1,
            Sequence{
                Not{
                    Sequence{
                        Inline{&noise},
                        OneOf{",]"}
                    }
                },
                NoneOf{",;{}:\"\n\r"}
            }
        }
    };

    SyntaxRule line {
        Sequence{
            Not{OneOf{"[{"}},
            Length{1,
                Find{
                    Ahead{
                        Sequence{
                            Repeat{
                                Choice{
                                    OneOf{" \t\r}"},
                                    Inline{&commentLine},
                                    Inline{&commentText}
                                }
                            },
                            Choice{
                                Ahead{'\n'},
                                Eoi{}
                            }
                        }
                    }
                }
            }
        }
    };

    SyntaxRule string {
        Sequence{
            '"',
            Repeat{
                Choice{
                    &escapedChar,
                    NoneOf{"\"\n"}
                }
            },
            Expect{'"'}
        }
    };

    SyntaxRule document {
        Find{Ahead{Replay{"eod"}}}
    };

    SyntaxRule text {
        Choice{
            Sequence{
                &string,
                Repeat{
                    Sequence{
                        Inline{&noise},
                        &string
                    }
                }
            },
            Sequence{
                "<<",
                Capture{Inline{&identifier}, "eod"},
                Repeat{OneOf{" \t"}},
                Repeat{0, 1, Inline{&commentLine}},
                '\n',
                &document,
                Replay{"eod"}
            },
            Context{&list,
                &item,
                &line
            }
        }
    };

    SyntaxRule boolean {
        Keyword{"true", "false"}
    };

    SyntaxRule hexDigit {
        Choice{
            Within{'0', '9'},
            Within{'A', 'F'},
            Within{'a', 'f'}
        }
    };

    SyntaxRule color {
        Sequence{
            '#',
            Choice{
                Repeat{8, 8, Inline{&hexDigit}},
                Repeat{6, 6, Inline{&hexDigit}},
                Repeat{3, 3, Inline{&hexDigit}}
            }
        }
    };

    SyntaxRule version {
        Sequence{
            Repeat{0, 1, 'v'},
            Repeat{1, 6, Within{'0', '9'}},
            '.',
            Repeat{1, 6, Within{'0', '9'}},
            Repeat{0, 1,
                Sequence{
                    '.',
                    Repeat{1, 6, Within{'0', '9'}}
                }
            }
        }
    };

    SyntaxRule identifier {
        Repeat{1,
            Choice{
                Within{'a', 'z'},
                Within{'A', 'Z'},
                OneOf{"_-"},
                Within{'0', '9'}
            }
        }
    };

    SyntaxRule name {
        Choice{
            Sequence{
                Not{Within{'0', '9'}},
                Inline{&identifier}
            },
            Inline{&string}
        }
    };

    SyntaxRule className {
        Sequence{
            Ahead{Within{'A', 'Z'}},
            Inline{&identifier}
        }
    };

    SyntaxRule member {
        Choice{
            &object,
            Sequence{
                &name,
                Inline{&noise},
                Expect{':'},
                Inline{&noise},
                Expect{Inline{&value}, "Illegal value syntax"}
            }
        }
    };

    SyntaxRule object {
        Sequence{
            Repeat{0, 1,
                Sequence{
                    &className,
                    Inline{&noise}
                }
            },
            '{',
            Inline{&noise},
            Repeat{0, 1,
                Sequence{
                    Inline{&member},
                    Repeat{
                        Sequence{
                            Length{1,
                                Sequence{
                                    Inline{&noise},
                                    Repeat{0, 1,
                                        Sequence{
                                            OneOf{",;"},
                                            Inline{&noise}
                                        }
                                    }
                                }
                            },
                            Inline{&member}
                        }
                    }
                }
            },
            Inline{&noise},
            Expect{'}'}
        }
    };

    SyntaxRule list {
        Sequence{
            '[',
            Inline{&noise},
            Repeat{0, 1,
                Sequence{
                    Inline{&value},
                    Repeat{
                        Sequence{
                            Inline{&noise},
                            OneOf{",;"},
                            Inline{&noise},
                            Inline{&value}
                        }
                    }
                }
            },
            Inline{&noise},
            Expect{']'}
        }
    };

    SyntaxRule value {
        Choice{
            &integer,
            &real,
            &color,
            &version,
            &object,
            &list,
            &boolean,
            &text
        }
    };

    SyntaxRule message {
        Sequence{
            Inline{&noise},
            Expect{Inline{&value}},
            Inline{&noise}
        }
    };

    State():
        SyntaxDefinition::State{&message}
    {}

    Variant parse(const String &text, const MetaProtocol &protocol) const
    {
        Token token = match(text);
        if (!token) throw TextError{text, token};
        Token valueToken = token.children().first();
        if (protocol) {
            MetaPrototype prototype;
            String className;
            readClassName(text, valueToken, &className);
            if (!protocol.lookup(className, &prototype)) {
                throw TextError{
                    text, valueToken.i0(),
                    Format{"Illegal message type \"%%\""} << className
                };
            }
            return readObject(text, valueToken, prototype);
        }
        return readValue(text, valueToken);
    }

    MetaObject readObject(const String &text, const Token &token, const MetaPrototype &prototype = MetaPrototype{}) const
    {
        if (token.rule() != object)
            throw TextError{text, token.i0(), "Expected an object value"};

        auto it = token.children().begin();
        String className;
        if (readClassName(text, token, &className)) ++it;

        MetaObject object;

        if (prototype) {
            if (className != prototype.className()) {
                throw TextError{
                    text, token.i0(),
                    Format{"Expected an object of class \"%%\""} << prototype.className()
                };
            }
            object = prototype.produce();
        }
        else {
            object = MetaObject{className};
        }

        for (; it; ++it)
        {
            Token child = *it;

            if (child.rule() == name) {
                String memberName = readName(text, child);

                Variant defaultValue;
                MetaPrototype memberPrototype;

                if (prototype) {
                    if (!prototype.members().lookup(memberName, &defaultValue)) {
                        throw TextError{
                            text, child.i1(),
                            Format{"Illegal member \"%%\" in %%"} << memberName << prototype.className()
                        };
                    }
                    if (defaultValue.is<MetaPrototype>()) {
                        memberPrototype = defaultValue.to<MetaPrototype>();
                    }
                    else if (defaultValue.is<MetaProtocol>()) {
                        MetaProtocol memberProtocol = defaultValue.to<MetaProtocol>();
                        String memberType;
                        readClassName(text, child, &memberType);
                        if (!memberProtocol.lookup(memberType, &memberPrototype)) {
                            throw TextError{
                                text, child.i0(),
                                Format{"Illegal member type \"%%\""} << memberType
                            };
                        }
                    }
                }

                Variant memberValue;

                ++it;
                child = *it;

                if (memberPrototype)
                    memberValue = readObject(text, child, memberPrototype);
                else
                    memberValue = readValue(text, child, defaultValue);

                if (!object.members().insert(memberName, memberValue)) {
                    throw TextError{
                        text, child.i1(),
                        Format{"Ambiguous value for member \"%%\""} << memberName,
                    };
                }
            }
            else {
                MetaProtocol prototypeProtocol;
                if (prototype) prototypeProtocol = prototype.protocol();
                if (prototypeProtocol) {
                    if (object.children().count() >= prototypeProtocol.maxCount()) {
                        throw TextError{
                            text, child.i0(),
                            Format{"Maximum number of children exceeded"}
                       };
                    }
                }
                MetaPrototype childPrototype;
                if (prototypeProtocol) {
                    String childType;
                    if (child.children().count() > 0) {
                        readClassName(text, child, &childType);
                    }
                    if (!prototypeProtocol.lookup(childType, &childPrototype)) {
                        throw TextError{
                            text, child.i0(),
                            Format{"Illegal member type \"%%\""} << childType
                        };
                    }
                }
                MetaObject childObject = readObject(text, child, childPrototype);
                object.children().append(childObject);
            }
        }

        if (prototype) {
            prototype.autocomplete(&object);

            MetaProtocol prototypeProtocol = prototype.protocol();
            if (prototypeProtocol) {
                if (prototypeProtocol.minCount() > 0) {
                    if (object.children().count() < prototypeProtocol.minCount()) {
                        if (prototypeProtocol.prototypes().count() == 1 && prototypeProtocol.minCount() == 1) {
                            throw TextError{
                                text, token.i0(),
                                Format{"Object of type %% needs at least one child of type %%"} << object.className() << prototypeProtocol.prototypes().first().key()
                            };
                        }
                        else {
                            throw TextError{
                                text, token.i0(),
                                Format{"Object of type %% needs at least %% children"} << object.className() << prototypeProtocol.minCount()
                            };
                        }
                    }
                }
            }
        }

        try {
            object.realize();
        }
        catch (MetaError &error) {
            Token errorToken = token;
            if (error.offendingObject() != object) {
                for (long i = 0; i < object.children().count(); ++i) {
                    if (object.children().at(i) == error.offendingObject()) {
                        errorToken = getChildToken(token, i);
                        break;
                    }
                }
            }

            if (errorToken) {
                if (error.memberName() != "")
                    errorToken = getMemberValueToken(text, errorToken, error.memberName());
            }
            else
                errorToken = token;

            throw TextError{
                text, errorToken.i1(),
                error.message()
            };
        }

        return object;
    }

    Token getMemberValueToken(const String &text, const Token &objectToken, const String &memberName) const
    {
        Token token;
        for (auto it = objectToken.children().begin(); it; ++it) {
            token = *it;
            if (token.rule() == this->name) {
                if (readName(text, token) == memberName) {
                    ++it;
                    if (it) token = *it;
                    break;
                }
            }
        }
        return token;
    }

    Token getChildToken(const Token &objectToken, long childIndex) const
    {
        for (const Token &token: objectToken.children()) {
            if (token.rule() == this->object) {
                if (childIndex == 0) return token;
                --childIndex;
            }
        }
        return Token{};
    }

    bool readClassName(const String &text, const Token &token, Out<String> className) const
    {
        if (token.children().count() > 0) {
            Token child = token.children().first();
            if (child && child.rule() == this->className) {
                className = text.copy(child);
                return true;
            }
        }
        return false;
    }

    String readName(const String &text, const Token &token) const
    {
        bool stripQuotation = (text.at(token.i0()) == '"');
        return text.copy(token.i0() + stripQuotation, token.i1() - stripQuotation);
    }

    template<class T>
    List<T> readList(const String &text, const Token &token) const
    {
        Variant itemPrototype{T{}};
        List<T> list;
        for (const Token &child: token.children()) {
            Variant itemValue = readValue(text, child, itemPrototype);
            list.append(itemValue.to<T>());
        }
        return list;
    }

    Variant readList(const String &text, const Token &token, const Variant &prototype) const
    {
        Variant value;
        if (prototype.is<void>() || prototype.is<List<Variant>>()) {
            Variant itemPrototype;
            List<Variant> list;
            for (const Token &child: token.children()) {
                list.append(readValue(text, child, itemPrototype));
            }
            value = list;
        }
        else if (prototype.is<List<String>>()) value = readList<String>(text, token);
        else if (prototype.is<List<double>>()) value = readList<double>(text, token);
        else if (prototype.is<List<long>>()) value = readList<long>(text, token);
        else if (prototype.is<List<bool>>()) value = readList<bool>(text, token);
        return value;
    }

    Variant readValue(const String &text, const Token &token, const Variant &prototype = Variant{}) const
    {
        Variant value;

        if (token.rule() == real)
        {
            double x = 0;
            floatSyntax.read(&x, text, token);
            value = x;
        }
        else if (token.rule() == integer)
        {
            uint64_t x; int s;
            integerSyntax.read(&x, &s, text, token);
            if (prototype.is<double>())
                value = double(x) * s;
            else
                value = long(x) * s;
        }
        else if (token.rule() == color)
        {
            value = Color{text.copy(token)};
        }
        else if (token.rule() == version)
        {
            if (prototype.is<Version>())
                value = Version{text.copy(token)};
            else
                value = text.copy(token);
        }
        else if (token.rule() == this->text)
        {
            String chunk = readText(text, token);
            if (prototype.is<Color>())
                value = Color{chunk};
            else
                value = chunk;
        }
        else if (token.rule() == object)
        {
            value = readObject(text, token);
        }
        else if (token.rule() == list)
        {
            value = readList(text, token, prototype);
        }
        else if (token.rule() == boolean)
        {
            value = (text.copy(token) == "true");
        }

        if (!prototype.is<void>() && !prototype.sameTypeAs(value))
        {
            fixTypeMismatch(prototype, value);

            if (!prototype.sameTypeAs(value)) {
                throw TextError{
                    text, token.i0(),
                    Format{"Expected a value of type %%"} << prototype.typeName()
                };
            }
        }

        return value;
    }

    static void fixTypeMismatch(const Variant &prototype, Variant &value)
    {
        if (prototype.is<String>()) value = str(value);
        else if (prototype.is<double>() && value.is<long>()) value = value.to<double>();
        else if (prototype.is<List<String>>()) value = List<String>{value.is<String>() ? value.to<String>() : str(value)};
        else if (prototype.is<List<double>>()) {
            if (value.is<double>() || value.is<long>())
                value = List<double>{value.to<double>()};
        }
        else if (prototype.is<List<long>>()) {
            if (value.is<long>())
                value = List<long>{value.to<long>()};
        }
        else if (prototype.is<List<bool>>()) {
            if (value.is<bool>())
                value = List<bool>{value.to<bool>()};
        }
        else if (prototype.is<List<Variant>>()) {
            value = List<Variant>{value};
        }
    }

    String readText(const String &text, const Token &token) const
    {
        Token child = token.children().first();

        String value;

        if (child.rule() != string) {
            value = text.copy(child.i0(), child.i1());
        }
        else {
            List<String> parts;
            for (const Token &child: token.children()) {
                parts.append(text.copy(child.i0() + 1, child.i1() - 1));
            }
            value = (parts.count() == 1) ? parts.at(0) : parts.join();
            value.expand();
        }

        return value;
    }
};

YasonSyntax::YasonSyntax()
{
    initOnce<State>();
}

Variant YasonSyntax::parse(const String &text, const MetaProtocol &protocol) const
{
    return me().parse(text, protocol);
}

MetaObject YasonSyntax::readObject(const String &text, const Token &token, const MetaPrototype &prototype) const
{
    return me().readObject(text, token, prototype);
}

const YasonSyntax::State &YasonSyntax::me() const
{
    return Object::me.as<State>();
}

} // namespace cc
