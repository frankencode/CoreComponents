/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/can/exceptions>
#include <cc/Format>

namespace cc {
namespace can {

String CanAbort::message() const { return Format{"CAN abort (%%)"} << reason() ; }
String CanTimeout::message() const { return "CAN timeout"; }
String CanDisconnect::message() const { return "CAN disconnect"; }
String CanUnexpected::message() const { return "CAN unexpected"; }

}} // namespace cc::can
