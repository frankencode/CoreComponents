/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/PasswordInput>
#include <cc/PasswordInputState>

namespace cc {

PasswordInput::PasswordInput():
    TextInput{onDemand<State>}
{}

PasswordInput &PasswordInput::associate(Out<PasswordInput> self)
{
    return View::associate<PasswordInput>(self);
}

String PasswordInput::password() const
{
    return editor().as<PasswordEditor>().password();
}

} // namespace cc
