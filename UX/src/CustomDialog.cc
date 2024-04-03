/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/CustomDialog>
#include <cc/ColumnLayout>

namespace cc {

CustomDialog::State::State()
{
    size([this]{
        return getSize();
    });

    add(
        contentArea_
        .pos({sp(16), sp(16)})
        .paper([this]{ return theme().dialogColor(); })
        .layout(
            ColumnLayout{}
            .margin(0)
        )
    );
}

double CustomDialog::State::preferredWidth() const
{
    return sp(280);
}

Size CustomDialog::State::getSize() const
{
    double w = contentArea_.width() + sp(32);
    if (w < preferredWidth()) w = preferredWidth();

    return Size{
        w,
        contentArea_.height() + buttonArea().height() + sp(32)
    };
}

Size CustomDialog::State::preferredSize() const
{
    return getSize();
}

Size CustomDialog::State::minSize() const
{
    return getSize();
}

CustomDialog::CustomDialog():
    Dialog{onDemand<State>}
{}

CustomDialog &CustomDialog::associate(Out<CustomDialog> self)
{
    return View::associate<CustomDialog>(self);
}

CustomDialog &CustomDialog::addContent(const View &view)
{
    me().contentArea_.add(view);
    return *this;
}

} // namespace cc
