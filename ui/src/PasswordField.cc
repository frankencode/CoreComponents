/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/PasswordField>
#include <cc/ui/PasswordInput>
#include <cc/ui/PasswordInputState>

namespace cc::ui {

PasswordField::PasswordField(const String &label, Out<PasswordField> self):
    TextField{PasswordInput{}, label}
{
    self = weak<PasswordField>();
}

String PasswordField::password() const
{
    return input().as<PasswordInput>().password();
}

} // namespace cc::ui
