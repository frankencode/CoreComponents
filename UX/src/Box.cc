/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Box>

namespace cc {

Box::State::State()
{
    paper([this]{
        return radius() > 0 ?
            (decoration() ? Color::Transparent : basePaper()) :
            color();
    });

    paint([this]{
        Painter p{this};

        const double w = size()[0];
        const double h = size()[1];
        const double r = radius();

        if (r == 0)
            ;
        else if (r == w / 2) {
            p.moveTo(Point{0, r});
            p.arc(Point{r, r}, r, rad(180), rad(360));
            p.lineTo(Point{w, h - r});
            p.arc(Point{r, h - r}, r, rad(0), rad(180));
            p.setPen(color());
            p.fill();
        }
        else if (r == h / 2) {
            p.moveTo(Point{r, h});
            p.arc(Point{r, r}, r, rad(90), rad(270));
            p.lineTo(Point{w - r, 0});
            p.arc(Point{w - r, r}, r, rad(270), rad(90));
            p.setPen(color());
            p.fill();
        }
        else {
            p.moveTo(Point{w - r, 0});
            p.arc(Point{w - r, r}, r, rad(270), rad(360));
            p.lineTo(Point{w, h - r});
            p.arc(Point{w - r, h - r}, r, rad(0), rad(90));
            p.lineTo(Point{r, h});
            p.arc(Point{r, h - r}, r, rad(90), rad(180));
            p.lineTo(Point{0, r});
            p.arc(Point{r, r}, r, rad(180), rad(270));
            p.setPen(color());
            p.fill();
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
