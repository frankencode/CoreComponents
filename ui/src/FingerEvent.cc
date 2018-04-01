/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Format>
#include <cc/ui/FingerEvent>

namespace cc {
namespace ui {

String FingerEvent::toString() const
{
    return Format()
        << "FingerEvent {" << nl
        << "  action: " << action_ << nl
        << "  time: " << time_ << nl
        << "  deviceId: " << deviceId_ << nl
        << "  fingerId: " << fingerId_ << nl
        << "  pos: " << pos_ << nl
        << "  step: " << step_ << nl
        << "  pressure: " << pressure_ << nl
        << "}";
}

}} // namespace cc::ui
