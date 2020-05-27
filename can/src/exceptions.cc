/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/can/exceptions>
#include <cc/format>

namespace cc {
namespace can {

string CanAbort::message() const { return format{"CAN abort (%%)"} << reason() ; }
string CanTimeout::message() const { return "CAN timeout"; }
string CanDisconnect::message() const { return "CAN disconnect"; }
string CanUnexpected::message() const { return "CAN unexpected"; }

}} // namespace cc::can
