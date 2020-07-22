/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/Picture>

namespace cc {
namespace ui {

Picture::Instance::Instance(Visual *initialVisual):
    visual{initialVisual}
{
    build >>[=]{
        inheritPaper();

        size <<[=]{
            if (!visual()) return Size{};
            return visual()->size();
        };
    };

    paint >>[=]{
        Painter p{this};
        visual()->paint(p);
    };
}

bool Picture::Instance::isPainted() const
{
    return visual();
}

}} // namespace cc::ui
