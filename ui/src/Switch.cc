/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/Switch>
#include <cc/ui/DragArea>
#include <cc/ui/Box>
#include <cc/ui/Shadow>

namespace cc::ui {

struct Switch::State final: public InputControl::State
{
    State();

    void settled() override;

    Size preferredSize() const override;
    Size minSize() const override;
    Size maxSize() const override;

    Control delegate() const override;

    Property<bool> on { false };

    Property<double> trackRadius;
    Property<double> trackLength;
    Property<double> thumbRadius;
    Property<double> knobRadius;

    DragArea dragArea_;
    View thumb_;
};

Switch::State::State()
{
    thumbRadius([]{ return sp(24); });
    trackRadius([]{ return sp(6); });
    knobRadius([]{ return sp(10); });
    trackLength([this]{ return 2 * knobRadius(); });

    add(
        Box{}
        .color([this]{ return theme().switchTrackColor(on()); })
        .radius([this]{ return trackRadius(); })
        .pos([this]{ return Point{thumbRadius() - trackRadius()}; })
        .size([this]{ return Size{trackLength() + 2 * trackRadius(), 2 * trackRadius() }; })
    );

    add(
        DragArea{&dragArea_}
        .cursor(CursorShape::Hand)
        .onClicked([this]{
            on(!on);
            thumb_.pos([this]{ return Point{on() ? trackLength() : 0., 0.}; });
            return true;
        })
        .onKeyPressed([this](const KeyEvent &event){
            if (
                event.keyCode() == KeyCode::Key_Plus ||
                event.keyCode() == KeyCode::KeyPad_Plus ||
                event.keyCode() == KeyCode::Key_Right ||
                event.keyCode() == KeyCode::Key_Up ||
                event.keyCode() == KeyCode::Key_Minus ||
                event.keyCode() == KeyCode::KeyPad_Minus ||
                event.keyCode() == KeyCode::Key_Left ||
                event.keyCode() == KeyCode::Key_Down ||
                event.keyCode() == KeyCode::Key_Space
            ) {
                on(!on);
                return true;
            }
            return onKeyPressed(event);
        })
        .size([this]{ return size(); })
        .add(
            View{&thumb_}
            .size([this]{ return Size{2 * thumbRadius()}; })
            .pos([this]{ return Point{on() ? trackLength() : 0., 0.}; })
            .add(
                Box{}
                .color([this]{ return theme().switchFocusColor(); })
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
                .color([this]{ return theme().switchKnobColor(on()); })
                .radius([this]{ return knobRadius(); })
                .size([this]{ return Size{2 * knobRadius()}; })
                .centerInParent()
                .decorate(
                    Shadow{}
                    .color([this]{ return theme().switchKnobShadowColor(on()); })
                    .offset(Step{0, sp(2)})
                    .blurRadius(sp(3))
                )
            )
        )
    );

    size([this]{ return preferredSize(); });
}

void Switch::State::settled()
{
    thumb_.posEasing(Easing::Bezier{0.5, 0.0, 0.5, 1.0}, 0.1);
}

Size Switch::State::preferredSize() const
{
    return Size{trackLength() + 2 * thumbRadius(), 2 * thumbRadius()};
}

Size Switch::State::minSize() const
{
    return preferredSize();

}
Size Switch::State::maxSize() const
{
    return preferredSize();
}

Control Switch::State::delegate() const
{
    return dragArea_;
}

Switch::Switch():
    InputControl{onDemand<State>}
{}

Switch::Switch(Out<Switch> self):
    InputControl{new State}
{
    self = weak<Switch>();
}

bool Switch::value() const
{
    return me().on();
}

Switch &Switch::value(bool newValue)
{
    me().on(newValue);
    return *this;
}

Switch &Switch::value(Definition<bool> &&f)
{
    me().on(std::move(f));
    return *this;
}

Switch::State &Switch::me()
{
    return View::me().as<State>();
}

const Switch::State &Switch::me() const
{
    return View::me().as<State>();
}

} // namespace cc::ui
