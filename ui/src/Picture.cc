/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/Picture>

namespace cc::ui {

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

Picture::Picture(Out<Picture> self):
    View{new State}
{
    self = *this;
}

Picture::Picture(const Visual &visual, Color color, Out<Picture> self):
    View{new State{visual, color}}
{
    self = *this;
}

Picture::Picture(Ideographic ch, double size, Color color, Out<Picture> self):
    View{new State{style().ideograph(ch, size), color}}
{
    self = *this;
}

} // namespace cc::ui
