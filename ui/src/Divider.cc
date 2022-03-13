/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/Divider>

namespace cc::ui {

Divider::State::State()
{
    paper([this]{ return basePaper(); });
    color([this]{ return theme().dividerColor().on(paper()); });
    thickness([this]{ return style().dividerThickness(); });
    size([this]{ return preferredSize(); });
    paint([this]{
        Painter p{this};
        p.rectangle(Point{0, height() - thickness()}, size());
        p.setPen(color());
        p.fill();
    });
}

Size Divider::State::preferredSize() const
{
    return Size{
        hasParent() ? parent().innerWidth() : gu(10),
        std::ceil(thickness())
    };
}

Size Divider::State::minSize() const
{
    const double h = std::ceil(thickness());
    return Size{0, h};
}

Size Divider::State::maxSize() const
{
    const double h = std::ceil(thickness());
    return Size{std::numeric_limits<double>::max(), h};
}

Divider::Divider():
    View{onDemand<State>}
{}

Divider::Divider(Out<Divider> self):
    View{new State}
{
    self = *this;
}

} // namespace cc::ui
