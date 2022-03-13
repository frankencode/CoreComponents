/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/Slider>
#include <cc/ui/DragArea>
#include <cc/ui/Box>
#include <cc/ui/Shadow>
#include <cc/DEBUG>

namespace cc::ui {

struct Slider::State: public InputControl::State
{
    State()
    {
        thumbRadius([]{ return sp(24); });
        knobRadius([]{ return sp(10); });
        activeTrackRadius([]{ return sp(3); });
        inactiveTrackRadius([]{ return sp(2); });
        thumbCenter([this]{ return Point{ thumbRadius() + trackWidth() * (value() - min()) / (max() - min()), height() / 2 }; });

        size([this]{ return preferredSize(); });
        updateTrackWidth();

        DragArea{&dragArea_}
        .cursor([this]{
            return
                thumb_.containsLocal(dragArea_.mapToChild(thumb_, dragArea_.pointerPos())) ?
                Cursor{CursorShape::Hand} :
                Cursor{};
        })
        .onPointerPressed([this](const PointerEvent &){
            dragArea_.focus(true);
            return false;
        })
        .onKeyPressed([this](const KeyEvent &event){
            if (
                event.keyCode() == KeyCode::Key_Plus ||
                event.keyCode() == KeyCode::KeyPad_Plus ||
                event.keyCode() == KeyCode::Key_Right ||
                event.keyCode() == KeyCode::Key_Up
            ) {
                const double delta = (max() - min()) / 10;
                setValue(outputValue() + delta);
                return true;
            }
            else if (
                event.keyCode() == KeyCode::Key_Minus ||
                event.keyCode() == KeyCode::KeyPad_Minus ||
                event.keyCode() == KeyCode::Key_Left ||
                event.keyCode() == KeyCode::Key_Down
            ) {
                const double delta = (max() - min()) / 10;
                setValue(outputValue() - delta);
                return true;
            }
            return onKeyPressed(event);
        })
        .size([this]{ return size(); })
        .add(
            View{&thumb_}
            .size([this]{ return Size{2 * thumbRadius()}; })
            .add(
                Box{}
                .color([this]{ return theme().sliderFocusColor(); })
                .radius([this]{ return thumbRadius(); })
                .size([this]{ return thumb_.size(); })
                .opacity([this]{
                    double o = 0;
                    if (dragArea_.isDragged()) o = 0.1;
                    else if (focus()) o = 0.08;
                    else if (dragArea_.hover() || dragArea_.pressed()) o = 0.05;
                    return o;
                })
            )
            .add(
                Box{}
                .color([this]{ return theme().sliderKnobColor(); })
                .radius([this]{ return knobRadius(); })
                .size([this]{ return Size{2 * knobRadius()}; })
                .centerInParent()
                .decorate(
                    Shadow{}
                    .color([this]{ return theme().sliderKnobShadowColor(); })
                    .offset(Step{0, sp(2)})
                    .blurRadius(sp(3))
                )
            )
        );

        add(
            Box{}
            .color([this]{ return theme().sliderInactiveTrackColor(); })
            .radius([this]{ return inactiveTrackRadius(); })
            .size([this]{ return Size{ trackWidth() + 2 * inactiveTrackRadius(), 2 * inactiveTrackRadius() }; })
            .centerRight([this]{ return Point{ width() - thumbRadius() + inactiveTrackRadius(), height() / 2 }; })
        );

        add(
            View{}
            .size([this]{ return Size{ thumb_.pos().x() + 2 * activeTrackRadius(), 2 * activeTrackRadius() + 1 }; })
            .centerLeft([this]{ return Point{ thumbRadius() - activeTrackRadius(), height() / 2 }; })
            .clip(true)
            .add(
                Box{}
                .color([this]{ return theme().sliderActiveTrackColor(); })
                .radius([this]{ return activeTrackRadius(); })
                .size([this]{ return Size{ trackWidth() + 2 * activeTrackRadius(), 2 * activeTrackRadius() }; })
            )
        );

        add(
            dragArea_
        );

        outputValue(
            [this]{
                if (resizing_) return valueSaved_;
                double value = (thumb_.pos().x() / trackWidth()) * (max() - min()) + min();
                if (precision() > 0) {
                    double fraction = std::fmod(value, precision());
                    double half = precision() / 2;
                    if (fraction > 0) {
                        value -= fraction;
                        if (fraction > half) value += precision();
                    }
                    else {
                        value -= fraction;
                        if (fraction < -half) value -= precision();
                    }
                    if (value < min()) value = min();
                    else if (value > max()) value = max();
                }
                return value;
            }
        );

        size.onChanged([this]{
            double valueSaved = outputValue();
            updateTrackWidth();
            setValue(valueSaved);
        });
    }

    void updateTrackWidth()
    {
        valueSaved_ = outputValue();
        resizing_ = true;
        trackWidth = width() - 2 * thumbRadius();
        resizing_ = false;
    }

    Size preferredSize() const override
    {
        return Size{
            expandableWidth(sp(280)),
            minSize()[1]
        };
    }

    Size minSize() const override
    {
        return { 4 * thumbRadius(), 2 * thumbRadius() };
    }

    Size maxSize() const override
    {
        return { std::numeric_limits<double>::max(), preferredSize()[1] };
    }

    Control delegate() const override { return dragArea_; }

    void setValue(double newValue)
    {
        if (newValue < min()) newValue = min();
        else if (newValue > max()) newValue = max();
        value(newValue);
        thumb_.pos(thumbCenter() - thumb_.size() / 2);
    }

    Property<double> min { 0 };
    Property<double> max { 100 };
    Property<double> precision { 1 };
    Property<double> value { 0 };
    Property<double> outputValue { 0 };

    Property<double> thumbRadius;
    Property<double> knobRadius;
    Property<double> trackWidth;
    Property<double> activeTrackRadius;
    Property<double> inactiveTrackRadius;
    Property<Point> thumbCenter;

    Monitor outputValueMonitor;

    DragArea dragArea_;
    View thumb_;

    bool resizing_ { false };
    double valueSaved_ { 0 };
};

Slider::Slider():
    InputControl{onDemand<Slider::State>}
{}

Slider::Slider(Out<Slider> self):
    InputControl{new State}
{
    self = *this;
}

double Slider::min() const
{
    return me().min();
}

Slider &Slider::min(double newValue)
{
    me().min(newValue);
    return *this;
}

Slider &Slider::min(Definition<double> &&f)
{
    me().min(std::move(f));
    return *this;
}

double Slider::max() const
{
    return me().max();
}

Slider &Slider::max(double newValue)
{
    me().max(newValue);
    return *this;
}

Slider &Slider::max(Definition<double> &&f)
{
    me().max(std::move(f));
    return *this;
}

double Slider::precision() const
{
    return me().precision();
}

Slider &Slider::precision(double newValue)
{
    me().precision(newValue);
    return *this;
}

Slider &Slider::precision(Definition<double> &&f)
{
    me().precision(std::move(f));
    return *this;
}

double Slider::value() const
{
    return me().outputValue();
}

Slider &Slider::value(double newValue)
{
    me().setValue(newValue);
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

} // namespace cc::ui
