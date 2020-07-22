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

InputLine::Instance::Instance(double maxThickness)
{
    build >>[=]{
        inheritPaper();

        size <<[=]{
            return Size { parent()->size()[0], std::ceil(maxThickness) };
        };
    };

    paint >>[=]{
        Painter p{this};
        p->rectangle(Point{ 0, size()[1] - thickness() }, size());
        p->setSource(ink());
        p->fill();
    };
}

}} // namespace cc::ui
