/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Picture>

namespace cc {

Picture::State::State(const Visual &initialVisual, Color initialColor):
    visual{initialVisual},
    color{initialColor.isValid() ? initialColor : theme().primaryTextColor()}
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

Picture::State::State(const Image &image, Color initialColor):
    color{initialColor}
{
    size(image.size());
    this->image(image);

    paper(Color::Transparent);

    attach(Monitor{
        [this]{
            if (color().isValid()) this->image().tone(color());
        }
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

Picture::Picture(const Image &image, Color color):
    View{new State{image, color}}
{}

Picture &Picture::associate(Out<Picture> self)
{
    return View::associate<Picture>(self);
}

} // namespace cc
