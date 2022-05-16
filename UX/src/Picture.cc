/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Picture>

namespace cc {

Picture::State::State(const Visual &initVisual, Color initColor):
    visual{initVisual},
    color{initColor.isValid() ? initColor : theme().primaryTextColor()}
{
    size([this]{ return visual() ? visual().size() : Size{}; });
    paper([this]{ return visual() ? basePaper() : Color{}; });

    paint([this]{
        if (!visual()) return;
        Painter p{this};
        if (color()) p.setPen(color());
        visual().paint(p);
    });
}

Picture::Picture():
    View{onDemand<State>}
{}

Picture::Picture(const Visual &visual, Color color):
    View{new State{visual, color}}
{}

Picture::Picture(Ideographic ch, double size, Color color):
    View{new State{style().ideograph(ch, size), color}}
{}

Picture &Picture::associate(Out<Picture> self)
{
    return View::associate<Picture>(self);
}

} // namespace cc
