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

/** \brief Generate YSON representation of a meta object tree
  * \see JsonWriter
  */
class YasonWriter: public Object
{
public:
    static Ref<YasonWriter> create(Format format = Format(), String indent = "  ");
    void write(Variant value);

protected:
    YasonWriter(Format format, String indent);
    void writeValue(Variant value, int depth);
    void writeList(Variant value, int depth);
    bool isIdentifier(String name) const;
    void writeObject(Variant value, int depth);
    void writeIndent(int depth);

    template<class T>
    void writeTypedList(Variant value, int depth);

    Format format_;
    String indent_;
};

}} // namespace cc::meta

