/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Slider>
#include <cc/SliderControl>
#include <cc/NumberCellState>

namespace cc {

struct Slider::State: public View::State
{
    State()
    {
        add(
            SliderControl{}
            .associate(&slider_)
            .pos([this]{
                return Point{
                    margin() + (leading() ? leading().width()  : 0.) + spacing(),
                    0.
                };
            })
            .size([this]{
                return Size{
                    width()
                        - (leading() ? leading().width() : 0.)
                        - (trailing() ? trailing().width() : 0.)
                        - 2 * spacing()
                        - 2 * margin(),
                    height()
                };
            })
        );

        size([this]{ return preferredSize(); });
    }

    Size preferredSize() const override
    {
        Size s = slider_.preferredSize();
        s[0] += 2 * spacing();
        s[0] += 2 * margin();
        if (leading()) s[0] += leading().preferredSize()[0];
        if (trailing()) s[0] += trailing().preferredSize()[0];
        if (hasParent() && parent().layout() && !parent().layout().isManaged()) s[0] = parent().innerWidth();
        return s;
    }

    Size minSize() const override
    {
        Size s = slider_.minSize();
        s[0] += 2 * spacing();
        s[0] += 2 * margin();
        if (leading()) s[0] += leading().preferredSize()[0];
        if (trailing()) s[0] += trailing().preferredSize()[0];
        return s;
    }

    void setLeading(View view)
    {
        assert(!leading());
        if (leading()) return;
        view.touch();
        add(view);
        view.centerLeft([this]{ return Point{margin(), height()/2}; });
        leading(view);
        autoConnectNumberCell(view);
    }

    void setTrailing(View view)
    {
        assert(!trailing());
        if (trailing()) return;
        view.touch();
        add(view);
        view.centerRight([this]{ return Point{width() - margin(), height()/2}; });
        trailing(view);
        autoConnectNumberCell(view);
    }

    void autoConnectNumberCell(View view)
    {
        if (view.is<NumberCell>()) {
            assert(!cell_);
            if (!cell_) {
                cell_ = view.as<NumberCell>();
                if (cell_->autoConnect_) {
                    cell_.min([this]{ return slider_.min(); });
                    cell_.max([this]{ return slider_.max(); });
                    cell_.precision([this]{ return slider_.precision(); });
                    cell_.value(slider_.value());
                    cell_.onValueChanged([this]{ slider_.value(cell_.value()); });
                    slider_.onValueChanged([this]{ cell_.value(slider_.value()); });
                }
            }
        }
    }

    Property<double> spacing { sp(4) };
    Property<double> margin { sp(8) };
    Property<View> leading;
    Property<View> trailing;
    SliderControl slider_;
    NumberCell cell_;
    Property<void> sliderFeed;
    Property<void> cellFeed;
};

Slider::Slider():
    View{onDemand<State>}
{}

Slider &Slider::associate(Out<Slider> self)
{
    return View::associate<Slider>(self);
}

Slider &Slider::leading(const View &view)
{
    me().setLeading(view);
    return *this;
}

Slider &Slider::trailing(const View &view)
{
    me().setTrailing(view);
    return *this;
}

double Slider::min() const
{
    return me().slider_.min();
}

Slider &Slider::min(double newValue)
{
    me().slider_.min(newValue);
    return *this;
}

Slider &Slider::min(Definition<double> &&f)
{
    me().slider_.min(move(f));
    return *this;
}

double Slider::max() const
{
    return me().slider_.max();
}

Slider &Slider::max(double newValue)
{
    me().slider_.max(newValue);
    return *this;
}

Slider &Slider::max(Definition<double> &&f)
{
    me().slider_.max(move(f));
    return *this;
}

double Slider::precision() const
{
    return me().slider_.precision();
}

Slider &Slider::precision(double newValue)
{
    me().slider_.precision(newValue);
    return *this;
}

Slider &Slider::precision(Definition<double> &&f)
{
    me().slider_.precision(move(f));
    return *this;
}

double Slider::value() const
{
    return me().slider_.value();
}

bool Slider::jumped() const
{
    return me().slider_.jumped();
}

Slider &Slider::value(double newValue)
{
    me().slider_.value(newValue);
    if (me().cell_) me().cell_.value(newValue);
    return *this;
}

Slider &Slider::onValueChanged(Fun<void()> &&f)
{
    me().slider_.onValueChanged(move(f));
    return *this;
}

Slider::State &Slider::me()
{
    return View::me().as<State>();
}

const Slider::State &Slider::me() const
{
    return View::me().as<State>();
}

} // namespace cc
