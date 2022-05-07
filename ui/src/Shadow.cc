/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Shadow>
#include <cc/shadowBlur>
#include <cmath>

namespace cc {

struct Shadow::State final: public View::State
{
    State()
    {
        moving([this]{
            if (!parent_()) return false;
            return parent_()->moving();
        });

        pos([this]{
            const auto r = blurRadius();
            return offset() - Point{r, r};
        });

        size([this]{
            if (!parent_()) return Size{};
            const auto d = std::ceil(2 * blurRadius());
            return parent_()->size() + Size{d, d};
        });

        size.onChanged([this]{ image(); });

        paint([this]{
            if (!parent_()) return;
            if (!image_) return;
            image_.clear(Color::Transparent);
            int r = std::round(blurRadius());
            parent_()->image().copyToXy(&image_, r, r);
            if (!parent_()->isOpaque()) image_.normalize();
            image_.shadowBlur(r, color());
            image_.premultiply();
        });
    }

    bool isOpaque() const override { return false; }
    bool isPainted() const override { return true; }

    Property<Step> offset;
    Property<double> blurRadius { sp(5) };
    Property<Color> color { 0x80000000u };
};

Shadow::Shadow():
    View{onDemand<State>}
{}

Shadow::Shadow(Out<Shadow> self):
    View{new State}
{
    self = weak<Shadow>();
}

Step Shadow::offset() const
{
    return me().offset();
}

Shadow &Shadow::offset(const Step &newValue)
{
    me().offset(newValue);
    return *this;
}

Shadow &Shadow::offset(Definition<Step> &&f)
{
    me().offset(std::move(f));
    return *this;
}

double Shadow::blurRadius() const
{
    return me().blurRadius();
}

Shadow &Shadow::blurRadius(double newValue)
{
    me().blurRadius(newValue);
    return *this;
}

Shadow &Shadow::blurRadius(Definition<double> &&f)
{
    me().blurRadius(std::move(f));
    return *this;
}

Color Shadow::color() const
{
    return me().color();
}

Shadow &Shadow::color(Color newValue)
{
    me().color(newValue);
    return *this;
}

Shadow &Shadow::color(Definition<Color> &&f)
{
    me().color(std::move(f));
    return *this;
}

Shadow::State &Shadow::me()
{
    return View::me().as<State>();
}

const Shadow::State &Shadow::me() const
{
    return View::me().as<State>();
}

} // namespace cc
