/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Format>
#include <cc/ui/MouseWheelEvent>

namespace cc {
namespace ui {

String MouseWheelEvent::toString() const
{
    return Format()
        << "MouseWheelEvent {" << nl
        << "  time: " << time_ << nl
        << "  wheelStep: " << wheelStep_ << nl
        << "  mousePos: " << mousePos_ << nl
        << "}";
}

}} // namespace cc::ui
