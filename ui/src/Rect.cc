/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Format>
#include <cc/ui/Rect>

namespace cc {
namespace ui {

void Rect::inset(int margin)
{
    if (2 * margin < w_) {
        w_ -= 2 * margin;
        x_ += margin;
    }
    else {
        x_ += w_ / 2;
        w_ = 0;
    }

    if (2 * margin < h_) {
        h_ -= 2 * margin;
        y_ += margin;
    }
    else {
        y_ += h_ / 2;
        h_ = 0;
    }
}

String Rect::toString() const
{
    return Format("Rect(x: %%, y: %%, w: %%, h: %%)") << x_ << y_ << w_ << h_;
}

}} // namespace cc::ui
