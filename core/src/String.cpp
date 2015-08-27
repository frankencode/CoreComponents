/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/Format>
#include <flux/Variant>
#include <flux/String>

namespace flux {

String::String(): Super(ByteArray::create()) {}

String::String(const Variant &b)
    : Super(cast<ByteArray>(b))
{
    if (!Super::get()) Super::set(ByteArray::create());
}

String::String(const Format &b)
{
    *this = *ByteArray::join(b);
}

String::String(Ref<StringList> parts) { *this = join(parts); }

} // namespace flux
