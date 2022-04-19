/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/SliderField>
#include <cc/ui/Slider>
#include <cc/ui/NumberCellState>

namespace cc::ui {

struct SliderField::State: public View::State
{
    State()
    {
        add(
            Slider{&slider_}
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
                    sliderFeed([this]{ slider_.value(cell_.value()); });
                    cellFeed([this]{ cell_.value(slider_.value()); });
                }
            }
        }
    }

    Property<double> spacing { sp(4) };
    Property<double> margin { sp(8) };
    Property<View> leading;
    Property<View> trailing;
    Slider slider_;
    NumberCell cell_;
    Property<void> sliderFeed;
    Property<void> cellFeed;
};

SliderField::SliderField():
    View{onDemand<State>}
{}

SliderField::SliderField(Out<SliderField> self):
    View{new State}
{
    self = *this;
}

SliderField &SliderField::leading(const View &view)
{
    me().setLeading(view);
    return *this;
}

SliderField &SliderField::trailing(const View &view)
{
    me().setTrailing(view);
    return *this;
}

double SliderField::min() const
{
    return me().slider_.min();
}

SliderField &SliderField::min(double newValue)
{
    me().slider_.min(newValue);
    return *this;
}

SliderField &SliderField::min(Definition<double> &&f)
{
    me().slider_.min(std::move(f));
    return *this;
}

double SliderField::max() const
{
    return me().slider_.max();
}

SliderField &SliderField::max(double newValue)
{
    me().slider_.max(newValue);
    return *this;
}

SliderField &SliderField::max(Definition<double> &&f)
{
    me().slider_.max(std::move(f));
    return *this;
}

double SliderField::precision() const
{
    return me().slider_.precision();
}

SliderField &SliderField::precision(double newValue)
{
    me().slider_.precision(newValue);
    return *this;
}

SliderField &SliderField::precision(Definition<double> &&f)
{
    me().slider_.precision(std::move(f));
    return *this;
}

double SliderField::value() const
{
    return me().slider_.value();
}

SliderField &SliderField::value(double newValue)
{
    me().slider_.value(newValue);
    return *this;
}

SliderField::State &SliderField::me()
{
    return View::me().as<State>();
}

const SliderField::State &SliderField::me() const
{
    return View::me().as<State>();
}

} // namespace cc::ui
