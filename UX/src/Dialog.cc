/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Dialog>
#include <cc/Box>
#include <cc/TextButton>
#include <cc/RowLayout>

namespace cc {

Dialog::State::State()
{
    add(
        Box{}
        .radius(sp(8))
        .color([this]{ return theme().dialogColor(); })
        .paper(Color::Transparent)
        .size([this]{ return size(); })
        .add(
            buttonArea_
            .layout(
                RowLayout{}
                .margin(Size{sp(8), sp(8)})
                .spacing(0)
            )
            .bottomRight(
                [this]{ return size(); }
            )
        )
    );
}

void Dialog::State::addAction(const Action &action)
{
    buttonArea_.add(
        TextButton{action.title().upcased(), action.icon()}
    );
}

Dialog::Dialog():
    View{onDemand<State>}
{}

Dialog &Dialog::addAction(const Action &action)
{
    me().addAction(action);
    return *this;
}

Dialog::State &Dialog::me()
{
    return get<State>();
}

const Dialog::State &Dialog::me() const
{
    return get<State>();
}

} // namespace cc
