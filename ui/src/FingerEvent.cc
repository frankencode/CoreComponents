/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/format>
#include <cc/ui/FingerEvent>

namespace cc {
namespace ui {

string FingerEvent::toString() const
{
    return format{}
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
