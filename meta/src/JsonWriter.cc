/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
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
