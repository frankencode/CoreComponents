/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/WheelEvent>
#include <cc/Format>

namespace cc {
namespace ui {

String WheelEvent::toString() const
{
    return Format{}
        << "WheelEvent {" << nl
        << "  time: " << time_ << nl
        << "  wheelStep: " << wheelStep_ << nl
        << "  mousePos: " << mousePos_ << nl
        << "}";
}

}} // namespace cc::ui
