/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Format>
#include <cc/ui/KeyEvent>

namespace cc {
namespace ui {

String KeyEvent::toString() const
{
    return Format()
        << "KeyEvent {" << nl
        << "  action: " << action_ << nl
        << "  time: " << time_ << nl
        << "  repeatCount: " << repeatCount_ << nl
        << "  scanCode: " << scanCode_ << nl
        << "  keyCode: " << keyCode_ << nl
        << "  modifiers: " << modifiers_ << nl
        << "}";
}

}} // namespace cc::ui
