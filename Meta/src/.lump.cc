/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/MetaError>

namespace cc {

MetaError::MetaError(const String &message, const MetaObject &offendingObject, const String &memberName):
    UsageError{message},
    offendingObject_{offendingObject},
    memberName_{memberName}
{}

MetaObject MetaError::offendingObject() const
{
    return offendingObject_;
}

String MetaError::memberName() const
{
    return memberName_;
}

} // namespace cc
/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/yason>
#include <cc/YasonSyntax>
#include <cc/YasonWriter>
#include <cc/CaptureSink>

namespace cc {

Variant yasonParse(const String &message, const MetaProtocol &protocol)
{
    return YasonSyntax{}.parse(message, protocol);
}

String yasonStringify(const Variant &value)
{
    CaptureSink sink;
    YasonWriter{sink}.write(value);
    return sink.collect();
}

} // namespace cc
/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/MetaProtocolState>

namespace cc {

MetaProtocol::MetaProtocol(New):
    Object{new State}
{}

MetaProtocol::MetaProtocol(State *newState):
    Object{newState}
{}

void MetaProtocol::inherit(const MetaProtocol &parent)
{
    me().parent_ = parent;
}

void MetaProtocol::define(const MetaPrototype &prototype)
{
    me().define(prototype);
}

const Map<String, MetaPrototype> &MetaProtocol::prototypes() const
{
    if (!Object::me) {
        thread_local Map<String, MetaPrototype> dummy;
        return dummy;
    }
    return me().prototypes_;
}

long MetaProtocol::minCount() const
{
    return (Object::me) ? me().minCount_ : 0;
}

long MetaProtocol::maxCount() const
{
    return (Object::me) ?  me().maxCount_ : 0;
}

void MetaProtocol::setMinCount(long newValue)
{
    me().minCount_ = newValue;
}

void MetaProtocol::setMaxCount(long newValue)
{
    me().maxCount_ = newValue;
}

bool MetaProtocol::lookup(const String &className, Out<MetaPrototype> prototype) const
{
    return (Object::me) ? me().lookup(className, prototype) : false;
}

MetaObject MetaProtocol::defaultObject() const
{
    return (Object::me) ? me().defaultObject_ : MetaObject{};
}

void MetaProtocol::setDefaultObject(const MetaObject &object)
{
    me().defaultObject_ = object;
}

const MetaProtocol::State &MetaProtocol::me() const
{
    return Object::me.as<State>();
}

MetaProtocol::State &MetaProtocol::me()
{
    if (!Object::me) Object::me = new State;
    return Object::me.as<State>();
}

const TypeInfo VariantType<MetaProtocol>::info
{
    .typeName = "MetaProtocol",
    .str = [](const void *bytes) {
        //! \TODO
        return String{};
    },
    .cleanup = [](void *bytes) {
        static_cast<MetaProtocol *>(bytes)->~MetaProtocol();
    },
    .assign = [](void *dst, const void *src) {
        new(dst)MetaProtocol{*static_cast<const MetaProtocol *>(src)};
    }
};

} // namespace cc
/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/MetaObject>
#include <cc/MetaPrototype>
#include <cc/YasonWriter>
#include <cc/CaptureSink>

namespace cc {

void MetaObject::autocomplete(InOut<MetaObject> target) const
{
    if (members().count() != target().members().count()) {
        for (int i = 0, n = members().count(); i < n; ++i) {
            String name = members().at(i).key();
            if (target().members().count() <= i || target().members().at(i).key() != name) {
                Variant value = members().at(i).value();
                if (value.is<MetaProtocol>()) value = value.to<MetaProtocol>().defaultObject();
                else if (value.is<MetaPrototype>()) value = value.to<MetaPrototype>().as<MetaObject>();
                target().members().insert(name, value);
            }
        }
    }

    target().children().appendList(children());
}

MetaObject MetaObject::clone() const
{
    auto object = produce();
    autocomplete(&object);
    object.me().realize();
    return object;
}

const TypeInfo VariantType<MetaObject>::info
{
    .typeName = "MetaObject",
    .str = [](const void *bytes) {
        CaptureSink sink;
        YasonWriter{sink}.writeObject(*static_cast<const MetaObject *>(bytes));
        return sink.collect();
    },
    .cleanup = [](void *bytes) {
        static_cast<MetaObject *>(bytes)->~MetaObject();
    },
    .assign = [](void *dst, const void *src) {
        new(dst)MetaObject{*static_cast<const MetaObject *>(src)};
    },
    .equal = [](const void *a, const void *b) {
        return VariantType<MetaObject>::retrieve(a) == VariantType<MetaObject>::retrieve(b);
    }
};

} // namespace cc
/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/YasonWriter>
#include <cc/Format>
#include <cc/Color>
#include <cc/Version>

namespace cc {

struct YasonWriter::State: public Object::State
{
    State(const Stream &sink, const String &indent):
        format_{sink},
        indent_{indent}
    {}

    void writeValue(const Variant &value, int depth = 0)
    {
        if (
            value.is<long>() ||
            value.is<bool>() ||
            value.is<double>() ||
            value.is<Version>() ||
            value.is<Color>()
        ) {
            format_ << value;
        }
        else if (value.is<String>())
        {
            String s = value;
            if (s.find('\"')) s = s.replaced("\"", "\\\"");
            s = s.escaped();
            format_ << "\"" << s << "\"";
        }
        else if (value.is<MetaObject>()) {
            writeObject(value.to<MetaObject>(), depth);
        }
        else if (value.is<List<String>>()) {
            writeList(value.to<List<String>>(), depth);
        }
        else if (value.is<List<Variant>>()) {
            writeList(value.to<List<Variant>>(), depth);
        }
        else if (value.is<List<long>>()) {
            writeList(value.to<List<long>>(), depth);
        }
        else if (value.is<List<bool>>()) {
            writeList(value.to<List<bool>>(), depth);
        }
        else if (value.is<List<double>>()) {
            writeList(value.to<List<double>>(), depth);
        }
        else {
            format_ << "\"" << str(value) << "\"";
        }
    }

    template<class T>
    void writeList(const List<T> &list, int depth = 0)
    {
        if (list.count() == 0) {
            format_ << "[]";
            return;
        }

        format_ << "[ ";

        for (const T &item: list) {
            writeValue(item, depth);
            if (!list.last(item))
                format_ << ", ";
        }

        format_ << " ]";
    }

    void writeObject(const MetaObject &object, int depth, const List<String> &names = List<String>{})
    {
        if (!object) {
            format_ << "null";
        }
        if (object.className() != "") {
            format_ << object.className() << " ";
        }
        if (object.members().count() == 0) {
            format_ << "{}";
            return;
        }
        format_ << "{\n";
        writeIndent(depth + 1);
        if (names.count() == 0) {
            for (const auto &pair: object.members()) {
                writeMember(pair.key(), pair.value(), object.members().last(pair), depth);
            }
        }
        else {
            for (const String &name: names) {
                writeMember(name, object.members().value(name), names.last(name), depth);
            }
        }
        writeChildren(object.children(), depth);
        writeIndent(depth);
        format_ << "}";
    }

    void writeMember(const String &name, const Variant &value, bool isLast, int depth)
    {
        if (isIdentifier(name))
            format_ << name << ": ";
        else
            format_ << "\"" << name << "\": ";

        writeValue(value, depth + 1);
        format_ << "\n";
        if (!isLast) writeIndent(depth + 1);
    }

    void writeChildren(const List<MetaObject> &children, int depth)
    {
        if (children.count() > 0) {
            writeIndent(depth + 1);
            for (const MetaObject &child: children) {
                writeObject(child, depth + 1);
                format_ << "\n";
                if (!children.last(child))
                    writeIndent(depth + 1);
            }
        }
    }

    void writeIndent(int depth)
    {
        if (indent_.count() > 0) {
            for (int i = 0; i < depth; ++i) {
                format_ << indent_;
            }
        }
    }

    static bool isIdentifier(const String &name)
    {
        for (char ch: name) {
            if (!(
                (('a' <= ch) && (ch <= 'z')) ||
                (('A' <= ch) && (ch <= 'Z')) ||
                (ch == '_') ||
                (ch == '-') ||
                (('0' <= ch) && (ch <= '9'))
            ))
                return false;
        }
        return true;
    }

    Format format_;
    String indent_;
};

YasonWriter::YasonWriter(const Stream &sink, const String &indent):
    Object{new State{sink, indent}}
{}

void YasonWriter::write(const Variant &value)
{
    me().writeValue(value);
}

void YasonWriter::writeObject(const MetaObject &object, const List<String> &names)
{
    me().writeObject(object, 0, names);
}

YasonWriter::State &YasonWriter::me()
{
    return Object::me.as<State>();
}

} // namespace cc
/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/json>
#include <cc/YasonSyntax>
#include <cc/JsonWriter>
#include <cc/CaptureSink>

namespace cc {

Variant jsonParse(const String &message)
{
    return YasonSyntax{}.parse(message);
}

String jsonStringify(const Variant &value)
{
    CaptureSink sink;
    JsonWriter{sink}.write(value);
    return sink.collect();
}

} // namespace cc
/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/MetaPrototype>

namespace cc {

MetaPrototype::MetaPrototype(const String &className):
    MetaObject{new State{className}}
{}

MetaPrototype::MetaPrototype(const String &className, const MetaProtocol &protocol):
    MetaObject{new State{className, protocol}}
{}

MetaPrototype::MetaPrototype(State *newState):
    MetaObject{newState}
{}

const MetaProtocol &MetaPrototype::protocol() const
{
    return me().protocol_;
}

const MetaPrototype::State &MetaPrototype::me() const
{
    return Object::me.as<State>();
}

const TypeInfo VariantType<MetaPrototype>::info
{
    .typeName = "MetaPrototype",
    .str = [](const void *bytes) {
        //! \TODO
        return String{};
    },
    .cleanup = [](void *bytes) {
        static_cast<MetaPrototype *>(bytes)->~MetaPrototype();
    },
    .assign = [](void *dst, const void *src) {
        new(dst)MetaPrototype{*static_cast<const MetaPrototype *>(src)};
    }
};

} // namespace cc
/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
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
/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/JsonWriter>
#include <cc/Format>

namespace cc {

struct JsonWriter::State: public Object::State
{
    State(const Stream &sink, const String &indent):
        format_{sink},
        indent_{indent}
    {}

    void writeValue(const Variant &value, int depth = 0)
    {
        if (value.is<long>() || value.is<bool>() || value.is<double>())
        {
            format_ << value;
        }
        else if (value.is<String>())
        {
            String s = value;
            if (s.find('\"')) s = s.replaced("\"", "\\\"");
            s = s.escaped();
            format_ << "\"" << s << "\"";
        }
        else if (value.is<MetaObject>()) {
            writeObject(value.to<MetaObject>(), List<String>{}, depth);
        }
        else if (value.is<List<String>>()) {
            writeList(value.to<List<String>>(), depth);
        }
        else if (value.is<List<Variant>>()) {
            writeList(value.to<List<Variant>>(), depth);
        }
        else if (value.is<List<long>>()) {
            writeList(value.to<List<long>>(), depth);
        }
        else if (value.is<List<bool>>()) {
            writeList(value.to<List<bool>>(), depth);
        }
        else if (value.is<List<double>>()) {
            writeList(value.to<List<double>>(), depth);
        }
        else {
            format_ << "\"" << str(value) << "\"";
        }
    }

    template<class T>
    void writeList(const List<T> &list, int depth = 0)
    {
        if (list.count() == 0) {
            format_ << "[]";
            return;
        }

        format_ << "[ ";

        for (const T &item: list) {
            writeValue(item, depth);
            if (&item != &list.last())
                format_ << ", ";
        }

        format_ << " ]";
    }

    void writeObject(const MetaObject &object, const List<String> &names, int depth = 0)
    {
        if (object.members().count() == 0) {
            format_ << "{}";
            return;
        }
        format_ << "{\n";
        writeIndent(depth + 1);
        if (names.count() == 0) {
            for (const auto &pair: object.members()) {
                writeMember(pair.key(), pair.value(), &pair == &object.members().last(), depth);
            }
        }
        else {
            for (const String &name: names) {
                writeMember(name, object.members().value(name), &name == &names.last(), depth);
            }
        }
        format_ << "}";
    }

    void writeMember(const String &name, const Variant &value, bool isLast, int depth)
    {
        format_ << "\"" << name << "\": ";
        writeValue(value, depth + 1);
        if (!isLast) {
            format_ << ",\n";
            writeIndent(depth + 1);
        }
        else {
            format_ << "\n";
            writeIndent(depth);
        }
    }

    void writeIndent(int depth)
    {
        for (int i = 0; i < depth; ++i) format_ << indent_;
    }

    Format format_;
    String indent_;
};

JsonWriter::JsonWriter(const Stream &sink, const String &indent):
    Object{new State{sink, indent}}
{}

void JsonWriter::write(const Variant &value)
{
    me().writeValue(value);
}

void JsonWriter::writeObject(const MetaObject &object, const List<String> &names)
{
    me().writeObject(object, names);
}

JsonWriter::State &JsonWriter::me()
{
    return Object::me.as<State>();
}

} // namespace cc
