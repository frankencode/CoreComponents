/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
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
