/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/debug>
#include <cc/meta/JsonWriter>

namespace cc {
namespace meta {

Ref<JsonWriter> JsonWriter::create(Stream *sink, String indent)
{
    return new JsonWriter(sink, indent);
}

JsonWriter::JsonWriter(Stream *sink, String indent):
    format_(sink),
    indent_(indent)
{}

void JsonWriter::write(Variant value)
{
    writeValue(value, 0);
    format_ << nl;
}

void JsonWriter::writeValue(Variant value, int depth)
{
    if (
        value->type() == VariantType::Int  ||
        value->type() == VariantType::Bool ||
        value->type() == VariantType::Float
    ) {
        format_ << value;
    }
    else if (value->type() == VariantType::String) {
        String s = value;
        if (s->contains("\""))
            s = s->replace("\"", "\\\"");
        s = s->escape();
        format_ << "\"" << s << "\"";
    }
    else if (value->type() == VariantType::List) {
        writeList(value, depth);
    }
    else if (value->type() == VariantType::Object) {
        writeObject(value, nullptr, depth);
    }
    else {
        format_ << "\"" << str(value) << "\"";
    }
}

void JsonWriter::writeList(Variant value, int depth)
{
    if (value->itemType() == VariantType::Int)
        writeTypedList<int>(value, depth);
    else if (value->itemType() == VariantType::Bool)
        writeTypedList<bool>(value, depth);
    else if (value->itemType() == VariantType::Float)
        writeTypedList<float>(value, depth);
    else if (value->itemType() == VariantType::String)
        writeTypedList<String>(value, depth);
    else
        writeTypedList<Variant>(value, depth);
}

void JsonWriter::writeObject(Variant value, const StringList *names, int depth)
{
    Ref<MetaObject> object = Variant::cast<MetaObject *>(value);
    if (object->count() == 0) {
        format_ << "{}";
        return;
    }
    format_ << "{\n";
    writeIndent(depth + 1);
    if (!names) {
        for (int i = 0; i < object->count(); ++i)
            writeMember(object->keyAt(i), object->valueAt(i), i == object->count() - 1, depth);
    }
    else {
        int i = 0;
        for (String name: names) {
            writeMember(name, object->value(name), i == names->count() - 1, depth);
            ++i;
        }
    }
    format_ << "}";
}

void JsonWriter::writeMember(String memberName, Variant memberValue, bool isLast, int depth)
{
    format_ << "\"" << memberName << "\": ";
    writeValue(memberValue, depth + 1);
    if (!isLast) {
        format_ << ",\n";
        writeIndent(depth + 1);
    }
    else {
        format_ << "\n";
        writeIndent(depth);
    }
}

void JsonWriter::writeIndent(int depth)
{
    for (int i = 0; i < depth; ++i) format_ << indent_;
}

template<class T>
void JsonWriter::writeTypedList(Variant value, int depth)
{
    List<T> *list = Variant::cast< List<T> *>(value);
    if (list->count() == 0) {
        format_ << "[]";
        return;
    }
    format_ << "[ ";
    for (int i = 0; i < list->count(); ++i) {
        writeValue(list->at(i), depth);
        if (i < list->count() - 1)
            format_ << ", ";
    }
    format_ << " ]";
}

}} // namespace cc::meta
