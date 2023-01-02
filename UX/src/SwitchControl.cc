/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/SwitchControl>
#include <cc/DragArea>
#include <cc/Box>
#include <cc/Shadow>

namespace cc {

struct SwitchControl::State final: public InputControl::State
{
    State();

    void settled() override;

    Size preferredSize() const override;
    Size minSize() const override;
    Size maxSize() const override;

    Control delegate() const override;

    void toggle();

    Property<bool> on { false };
    Trigger userInput;

    Property<double> trackRadius;
    Property<double> trackLength;
    Property<double> thumbRadius;
    Property<double> knobRadius;

    DragArea dragArea_;
    View thumb_;
};

SwitchControl::State::State()
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
        dragArea_
        .dragInhibit(true) //! \todo create an Apple style safety switch which makes use of dragging (SafetySwitch)
        // .cursor(CursorShape::Hand)
        .onClicked([this]{ // a SafetySwitch would have no clicked event
            toggle();
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
                toggle();
                return true;
            }
            return onKeyPressed(event);
        })
        .size([this]{ return size(); })
        .add(
            thumb_
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
                    else if (focus() || dragArea_.pressed()) o = 0.08;
                    else if (dragArea_.hover()) o = 0.05;
                    return o;
                })
            )
            .add(
                Box{}
                .color([this]{ return theme().switchKnobColor(on()); })
                .radius([this]{ return knobRadius(); })
                .size([this]{ return Size{2 * knobRadius()}; })
                .centerInParent()
                .decoration(
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

void SwitchControl::State::settled()
{
    thumb_.posEasing(Easing::Bezier{0.5, 0.0, 0.5, 1.0}, 0.1);
}

Size SwitchControl::State::preferredSize() const
{
    return Size{trackLength() + 2 * thumbRadius(), 2 * thumbRadius()};
}

Size SwitchControl::State::minSize() const
{
    return preferredSize();

}
Size SwitchControl::State::maxSize() const
{
    return preferredSize();
}

Control SwitchControl::State::delegate() const
{
    return dragArea_;
}

void SwitchControl::State::toggle()
{
    on(!on());
    userInput();
}

SwitchControl::SwitchControl():
    InputControl{onDemand<State>}
{}

SwitchControl &SwitchControl::associate(Out<SwitchControl> self)
{
    return View::associate<SwitchControl>(self);
}

bool SwitchControl::value() const
{
    return me().on();
}

SwitchControl &SwitchControl::value(bool newValue)
{
    me().on(newValue);
    return *this;
}

SwitchControl &SwitchControl::value(Definition<bool> &&f)
{
    me().on(move(f));
    return *this;
}

SwitchControl &SwitchControl::onValueChanged(Function<void()> &&f)
{
    me().on.onChanged(move(f));
    return *this;
}

SwitchControl &SwitchControl::onUserInput(Function<void()> &&f)
{
    me().userInput.connect(move(f));
    return *this;
}

SwitchControl::State &SwitchControl::me()
{
    return View::me().as<State>();
}

const SwitchControl::State &SwitchControl::me() const
{
    return View::me().as<State>();
}

} // namespace cc
