/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/AlertDialog>
#include <cc/Text>

namespace cc {

struct AlertDialog::State final: public Dialog::State
{
    State(const String &initialText = String{})
    {
        add(
            Text{}
            .associate(&text_)
            .text(initialText)
            .maxWidth(sp(280-48))
            .color([this]{ return theme().primaryTextColor(); })
            .paper([this]{ return theme().dialogColor(); })
            .topLeft({sp(24), sp(24)})
        );

        size([this]{ return getSize(); });
    }

    Size getSize() const
    {
        return Size{sp(280), text_.height() + sp(48) + buttonArea().height() + text_.fontMetrics().descender() };
    }

    Size preferredSize() const override
    {
        return getSize();
    }

    Size minSize() const override
    {
        return getSize();
    }

    Size maxSize() const override
    {
        return getSize();
    }

    Text text_;
};

AlertDialog::AlertDialog():
    Dialog{onDemand<State>}
{}

AlertDialog::AlertDialog(const String &text):
    Dialog{new State{text}}
{}

AlertDialog &AlertDialog::associate(Out<AlertDialog> self)
{
    return View::associate<AlertDialog>(self);
}

String AlertDialog::text() const
{
    return me().text_.text();
}

AlertDialog &AlertDialog::text(const String &newValue)
{
    me().text_.text(newValue);
    return *this;
}

AlertDialog &AlertDialog::text(Definition<String> &&f)
{
    me().text_.text(move(f));
    return *this;
}

AlertDialog::State &AlertDialog::me()
{
    return get<State>();
}

const AlertDialog::State &AlertDialog::me() const
{
    return get<State>();
}

} // namespace cc
