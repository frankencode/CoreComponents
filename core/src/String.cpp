/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/Format>
#include <flux/Variant>
#include <flux/Singleton>
#include <flux/String>

namespace flux {

String::String(): Super(Singleton<ByteArray>::instance()) {}

String::String(const Variant &b)
	: Super(cast<ByteArray>(b))
{
	if (!Super::get()) Super::set(Singleton<ByteArray>::instance());
}

String::String(const Format &b)
{
	*this = *ByteArray::join(b);
}

String::String(Ref<StringList> parts) { *this = join(parts); }

} // namespace flux
