/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/InputLine>

namespace cc {
namespace ui {

InputLine::InputLine(View *parent, double maxThickness):
    View(parent)
{
    inheritPaper();

    size->bind([=]{
        return Size { parent->size()[0], std::ceil(maxThickness) };
    });
}

void InputLine::paint()
{
    Painter p(this);
    p->rectangle(Point{ 0, size()[1] - thickness() }, size());
    p->setSource(ink());
    p->fill();
}

}} // namespace cc::ui
