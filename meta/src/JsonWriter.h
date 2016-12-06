/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Format>
#include <cc/meta/MetaObject>

namespace cc {
namespace meta {

/** \class JsonWriter JsonWriter.h cc/meta/JsonWriter
  * \brief Generate JSON representation of a meta object tree
  * \see yason, YasonWriter
  */
class JsonWriter: public Object
{
public:
    /** Create a new JSON writer
      * \param sink data stream to write the formatted output to
      * \param indent whitespace to use for each indentation step
      * \return new object instance
      */
    static Ref<JsonWriter> create(Stream *sink, String indent = "  ");

    /** Convert a meta value into JSON notation
      * \param value meta-value
      */
    void write(Variant value);

private:
    JsonWriter(Stream *sink, String indent);
    void writeValue(Variant value, int depth);
    void writeList(Variant value, int depth);
    void writeObject(Variant value, int depth);
    void writeIndent(int depth);

    template<class T>
    void writeTypedList(Variant value, int depth);

    Format format_;
    String indent_;
};

}} // namespace cc::meta
