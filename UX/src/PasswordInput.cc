/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the Apache License version 2.0
 * (see CoreComponents/LICENSE-Apache-2.0).
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
