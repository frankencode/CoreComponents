/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/syntax/exceptions>
#include <cc/meta/MetaObject>
#include <cc/meta/MetaProtocol>

namespace cc {
namespace yason {

using namespace cc::meta;

Variant parse(const ByteArray *text, const MetaProtocol *protocol = 0);
String stringify(Variant value);

}} // namespace cc::yason

