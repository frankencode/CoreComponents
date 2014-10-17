/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_YASONWRITER_H
#define FLUX_YASONWRITER_H

#include <flux/MetaObject>

namespace flux {

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

} // namespace flux

#endif // FLUX_YASONWRITER_H
