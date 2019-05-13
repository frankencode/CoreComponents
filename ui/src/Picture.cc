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

Picture::Picture(View *parent, Visual *initialVisual):
    View{parent},
    visual{initialVisual}
{
    inheritPaper();

    size->bind([=]{
        if (!visual()) return Size{};
        return visual()->size();
    });
}

bool Picture::isPainted() const
{
    return visual();
}

void Picture::paint()
{
    Painter p{this};
    visual()->paint(p);
}

}} // namespace cc::ui
