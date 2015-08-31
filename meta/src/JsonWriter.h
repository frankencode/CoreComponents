/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXMETA_JSONWRITER_H
#define FLUXMETA_JSONWRITER_H

#include <flux/meta/MetaObject>

namespace flux {

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

} // namespace flux

#endif // FLUXMETA_JSONWRITER_H
