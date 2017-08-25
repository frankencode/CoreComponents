/*
 * Copyright (C) 2007-2017 Frank Mertens.
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
namespace meta {
namespace yason {

/** Convenience function to parse a YASON message
  * \param text YASON message
  * \param protocol message protocol
  * \return parsed value
  */
Variant parse(const ByteArray *text, const MetaProtocol *protocol = 0);

/** Convenience function to stringify a variant value
  * \param value variant value
  * \return YASON message
  */
String stringify(Variant value);

}}} // namespace cc::meta::yason
