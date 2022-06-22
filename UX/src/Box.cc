/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Box>
#include <cc/DEBUG>

namespace cc {

Box::State::State()
{
    paper([this]{
        Color c = radius() > 0 ? (decoration() ? Color::Transparent : basePaper()) : color();
        if (!c.isValid()) c = basePaper();
        if (!c.isOpaque()) return Color::Transparent; // FIXME
        return c;
    });

    paint([this]{
        Painter painter{this};

        const double b2 = std::ceil(border().isNull() ? 0 : border().lineWidth() / 2);
        const double b = 2 * b2;
        const double x = b2;
        const double y = b2;
        const double w = size()[0] - b;
        const double h = size()[1] - b;
        const double r = radius() - b2;

        if (x != 0 || y != 0) {
            painter.translate(Point{x, y});
        }

        if (r == 0) {
            return;
        }
        else if (r == w / 2) {
            painter
            .moveTo({0, r})
            .arc({r, r}, r, rad(180), rad(360))
            .lineTo({w, h - r})
            .arc({r, h - r}, r, rad(0), rad(180))
            .lineTo({0, r});
        }
        else if (r == h / 2) {
            painter
            .moveTo({r, h})
            .arc({r, r}, r, rad(90), rad(270))
            .lineTo({w - r, 0})
            .arc({w - r, r}, r, rad(270), rad(90))
            .lineTo({r, h});
        }
        else {
            painter
            .moveTo({w - r, 0})
            .arc({w - r, r}, r, rad(270), rad(360))
            .lineTo({w, h - r})
            .arc({w - r, h - r}, r, rad(0), rad(90))
            .lineTo({r, h})
            .arc({r, h - r}, r, rad(90), rad(180))
            .lineTo({0, r})
            .arc({r, r}, r, rad(180), rad(270))
            .lineTo({w - r, 0});
        }

        if (border().isNull()) {
            painter
            .setPen(color())
            .fill();
        }
        else {
            painter
            .setPen(color())
            .fill(Painter::CurrentPath::Preserve)
            .setPen(border())
            .stroke();
        }
    });
}

Box::Box():
    View{onDemand<Box::State>}
{}

Box::Box(double width, double height):
    View{new State}
{
    size(Size{width, height});
}

Box &Box::associate(Out<Box> self)
{
    return View::associate<Box>(self);
}

} // namespace cc
