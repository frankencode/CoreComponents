/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/PasswordField>
#include <cc/PasswordInput>
#include <cc/PasswordInputState>

namespace cc {

PasswordField::PasswordField(const String &label, Out<PasswordField> self):
    TextField{PasswordInput{}, label}
{
    self = weak<PasswordField>();
}

String PasswordField::password() const
{
    return input().as<PasswordInput>().password();
}

} // namespace cc
