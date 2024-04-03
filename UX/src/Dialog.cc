/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/Dialog>
#include <cc/Box>
#include <cc/TextButton>
#include <cc/RowLayout>
#include <cc/Blind>
#include <cc/Application>
#include <cc/Shadow>

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
                .margin(Size{})
                .spacing(0)
            )
            .bottomRight([this]{
                return size() - Point{sp(8), 0};
            })
        )
        .decoration(
            Shadow{}
            .color([this]{
                return theme().dialogShadowColor();
            })
            .blurRadius(sp(6))
            .offset({0, sp(2)})
        )
    );
}

void Dialog::State::addAction(const Action &action)
{
    buttonArea_.add(
        TextButton{action.title(), action.icon()}
        .text([=]{ return action.title(); })
        .onClicked([=]{ action(); })
        .padding(
            Padding{}
            .top(sp(8))
            .bottom(sp(8))
        )
    );
}

void Dialog::State::close()
{
    if (hasParent()) {
        parent().parent().pop();
    }
}

Dialog &Dialog::associate(Out<Dialog> self)
{
    return View::associate<Dialog>(self);
}

Dialog &Dialog::addAction(const Action &action)
{
    me().addAction(action);
    return *this;
}

Dialog &Dialog::open()
{
    assert(!hasParent());
    if (hasParent()) return *this;
    Window window = Application{}.appWindow();
    assert(window);
    if (!window) return *this;
    window.view().push(
        Blind{}
        .add(
            (*this)
            .centerInParent()
        )
    );
    return *this;
}

void Dialog::close()
{
    if (hasParent()) {
        parent().parent().pop();
    }
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
