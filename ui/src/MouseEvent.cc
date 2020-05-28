/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Format>
#include <cc/ui/MouseEvent>

namespace cc {
namespace ui {

String MouseEvent::toString() const
{
    return Format{}
        << "MouseEvent {" << nl
        << "  action: " << action_ << nl
        << "  time: " << time_ << nl
        << "  button: " << button_ << nl
        << "  clickCount: " << clickCount_ << nl
        << "  pos: " << pos_ << nl
        << "  step: " << step_ << nl
        << "}";
}

}} // namespace cc::ui
