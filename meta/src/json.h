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

namespace cc {
namespace meta {
namespace json {

/** Convenience function to parse a JSON message
  * \param text JSON message
  * \return parsed value
  */
Variant parse(const ByteArray *text);

/** Convenience function to stringify a variant value
  * \param value variant value
  * \return JSON message
  */
String stringify(Variant value);

}}} // namespace cc::meta::json
