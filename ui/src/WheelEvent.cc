/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/format>
#include <cc/ui/WheelEvent>

namespace cc {
namespace ui {

string WheelEvent::toString() const
{
    return format{}
        << "WheelEvent {" << nl
        << "  time: " << time_ << nl
        << "  wheelStep: " << wheelStep_ << nl
        << "  mousePos: " << mousePos_ << nl
        << "}";
}

}} // namespace cc::ui
