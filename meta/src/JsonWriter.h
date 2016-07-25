/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/meta/MetaObject>

namespace cc {
namespace meta {

/** \brief Generate JSON representation of a meta object tree
  * \see yason, YasonWriter
  */
class JsonWriter: public Object
{
public:
    static Ref<JsonWriter> create(Format format = Format(), String indent = "  ");
    void write(Variant value);

protected:
    JsonWriter(Format format, String indent);
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

